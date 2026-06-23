#include "server/includes/gameloop.h"

#include <algorithm>
#include <iostream>
#include <limits>
#include <memory>
#include <ranges>
#include <sstream>
#include <string>
#include <unordered_set>
#include <utility>

#include "common/includes/core/snapshot.h"
#include "common/includes/map/map.h"
#include "server/includes/commands/command.h"
#include "server/includes/commands/command_signup.h"
#include "server/includes/core/data.h"
#include "server/includes/entity/combat_entity.h"
#include "server/includes/exceptions/invalid_buy_exception.h"
#include "server/includes/exceptions/invalid_sell_exception.h"
#include "server/includes/game_message_builder.h"
#include "server/includes/npc/banker.h"
#include "server/includes/npc/creature.h"
#include "server/includes/npc/merchant.h"
#include "server/includes/npc/priest.h"
#include "server/includes/response_builder.h"
#include "server/includes/responses/response_bank_content.h"
#include "server/includes/responses/response_chat_msg.h"
#include "server/includes/responses/response_equipment_update.h"
#include "server/includes/responses/response_inventory_update.h"
#include "server/includes/responses/response_login.h"
#include "server/includes/responses/response_map.h"
#include "server/includes/responses/response_signup.h"
#include "server/includes/responses/response_snapshot.h"
#include "server/includes/responses/response_trader_catalog.h"
#include "server/print.h"

#define INVALID_REGISTER "Username already taken."
#define INVALID_LOGIN "El usuario o la contraseña son incorrectos."
#define INVALID_PASSWORD "Wrong password."

namespace {
constexpr uint32_t RESURRECTION_MS_PER_TILE = 250;
constexpr uint32_t MIN_RESURRECTION_DELAY_MS = 1000;
constexpr TypeItem DEBUG_EQUIPMENT_ITEM = NONE;  // prueba inventario
constexpr uint32_t CREATURE_MOVEMENT_COOLDOWN_MS = 375;
constexpr uint16_t MIN_CREATURE_AGGRO_RANGE = 8;
constexpr uint16_t PRIEST_DIALOG_FIRST = 620;
constexpr uint16_t PRIEST_DIALOG_LAST = 623;

SoundEffectID attackSoundFor(TypeItem weapon_type) {
    switch (weapon_type) {
        case SWORD:
            return SoundEffectID::ESPADAZO;
        case ASH_STAFF:
            return SoundEffectID::FLECHA_MAGICA;
        case KNOTTED_STAFF:
        case INLAID_STAFF:
            return SoundEffectID::RESORTE_EXPLOSIVO;
        case SIMPLE_BOW:
        case COMPOUND_BOW:
            return SoundEffectID::FLECHA;
        default:
            return SoundEffectID::GOLPE_ARMA;
    }
}

std::optional<VisualEffectID> attackVisualFor(TypeItem weapon_type) {
    switch (weapon_type) {
        case ASH_STAFF:
            return VisualEffectID::LIGHTNING;
        case KNOTTED_STAFF:
        case INLAID_STAFF:
            return VisualEffectID::EXPLOSION;
        default:
            return std::nullopt;
    }
}
}  // namespace

using RespSnapshot = std::shared_ptr<ResponseSnapshot>;
Gameloop::Gameloop(GameConfig&& conf_, MonitorQueues& monitor, QueueCmd& cmmds_queue):
        monitor(monitor),
        commands_queue(cmmds_queue),
        conf(std::move(conf_)),
        clan_manager(this->conf.paths.clans),
        world(this->conf.paths.map, this->conf.citizen_detail_spawns),
        persistence(this->conf.paths),
        spawn(this->conf, this->world) {
    this->clan_manager.load();

    if (persistence.worldStateExists()) {
        Print::printInitGameloop("CARGANDO WORLD");
        WorldStateData world_data = persistence.loadWorldState();
        this->loadCitizenNPCs(world_data);
        this->loadCreatures(world_data);
        this->loadItemsInTheFloor(world_data);
    } else {
        Print::printInitGameloop("INICIANDO WORLD");
        this->spawn.spawnCitizenNpcZones(this->next_npc_id, this->citizen_npcs, this->bank);
        this->spawn.spawnCreaturesZones(this->next_npc_id, this->creatures);
        this->spawn.spawnTreasuresZones();
    }
    Print::printMessageConsole("FINAL CONSTRUCTOR GAMELOOP");
}

void Gameloop::loadCreatures(const WorldStateData& world_data) {
    std::unordered_set<Id> protected_zones;
    for (const auto& spawn: this->world.getCitizenSpawnPoints()) {
        protected_zones.insert(spawn.zone_id);
    }
    const auto safe_zones = this->world.getSafeZones();
    for (const auto& data: world_data.creatures) {
        if (safe_zones.contains(data.zone_id) || protected_zones.contains(data.zone_id)) {
            continue;
        }
        this->next_npc_id += 1;
        auto creature = this->spawn.loadCreature(next_npc_id, data);
        this->creatures.emplace(this->next_npc_id, std::move(creature));
    }
    Print::printMessageConsole("SE TERMINO DE CARGAR LAS CRIATURAS");
}

void Gameloop::loadCitizenNPCs(const WorldStateData& world_data) {
    for (const auto& data: world_data.citizen) {
        this->next_npc_id += 1;
        auto citizen = this->spawn.loadCitizen(next_npc_id, data, this->bank);
        this->citizen_npcs.emplace(next_npc_id, std::move(citizen));
    }
    Print::printMessageConsole("SE TERMINO DE CARGAR LOS CIUDADANOS");
}

void Gameloop::loadItemsInTheFloor(const WorldStateData& world_data) {
    this->spawn.loadTreasuresZones(world_data.treasures);
    this->spawn.loadGoldBags(world_data.gold_bags);
    this->spawn.loadItems(world_data.items);
}


Character Gameloop::createCharacter(const CharacterTraits& traits) const {
    const auto type_race = static_cast<TypeRace>(traits.race);
    const auto type_clase = static_cast<TypeClase>(traits.clase);
    const Race& race = this->conf.races.at(type_race);
    const Clase& clase = this->conf.clases.at(type_clase);
    return Character(race, clase, traits.head, traits.body);
}

Equipment Gameloop::loadingEquipment(const PlayerData& player) const {
    Equipment result;
    for (const auto& data: player.equipment) {
        const auto type = static_cast<TypeItem>(data.type_item);
        if (type == NONE)
            continue;
        const auto item = this->conf.items.at(type).get();
        auto instance = std::make_unique<ItemInstance>(item);
        result.equipItem(std::move(instance));
    }
    return result;
}

Inventory Gameloop::loadingInventory(const PlayerData& player) const {
    Inventory result(player.golden, this->conf.player_init.max_slots,
                     this->conf.player_init.capacity_slot);
    for (const auto& data: player.inventory) {
        const auto type = static_cast<TypeItem>(data.type_item);
        if (type == NONE)
            continue;
        const auto item = this->conf.items.at(type).get();
        for (uint32_t i = 0; i < data.quantity; ++i) {
            result.addItemToInventory(item);
        }
    }
    return result;
}

void Gameloop::loadAccountBank(const PlayerData& player) {
    std::string username(player.username);
    const auto& slots = player.box;
    Account account(player.golden_dep);
    for (const auto& data: slots) {
        const auto type = static_cast<TypeItem>(data.type_item);
        const auto* item = this->conf.items.at(type).get();
        account.loadItemAccount(item, data.quantity);
    }
    this->bank.accounts[username] = std::move(account);
}

void Gameloop::loadingPlayerData(const Id& player_id, const PlayerData& player_data) {
    Character charact = this->createCharacter(player_data.charact_traits);
    Inventory inv = this->loadingInventory(player_data);
    Equipment equip = this->loadingEquipment(player_data);
    Position position = this->world.findNearbyFreePosition(player_data.position);
    auto dir = static_cast<Direction>(player_data.direction);
    Pose pose(position, dir);
    auto new_player = std::make_unique<Player>(pose, std::move(inv), std::move(equip),
                                               std::move(charact), player_data);

    this->players.emplace(player_id, std::move(new_player));
    this->world.addPlayerWorld(player_id, pose);
    this->loadAccountBank(player_data);
}

void Gameloop::createNewPlayer(const User& user, const CharacterTraits& traits) {
    Character ch = this->createCharacter(traits);
    const Position position_spawn = this->world.calculatePositionRandomSafeZone();
    Pose pose_spawn(position_spawn, DOWN);
    auto new_player =
            std::make_unique<Player>(User(user), pose_spawn, std::move(ch), this->conf.player_init);
    this->bank.accounts.emplace(user.username, Account(0));
    const PlayerData player_data = new_player->getPlayerData(this->bank);
    this->persistence.savePlayer(player_data);
}

void Gameloop::processHandleSignup(const Id& player_id, const User& user,
                                   const CharacterTraits& traits) {
    Print::printNewPlayerArrived(player_id, user, static_cast<TypeRace>(traits.race),
                                 static_cast<TypeClase>(traits.clase));
    if (this->persistence.exists(user.username)) {
        this->monitor.queueTheServerResponse(
                player_id, std::make_unique<ResponseSignup>(false, INVALID_REGISTER));
        return;
    }
    this->createNewPlayer(user, traits);
    this->monitor.queueTheServerResponse(player_id, std::make_unique<ResponseSignup>(true));
}

void Gameloop::processHandleLogin(const Id& player_id, const User& user) {
    if (!this->persistence.exists(user.username)) {
        this->monitor.queueTheServerResponse(
                player_id, std::make_unique<ResponseLogin>(false, player_id, INVALID_LOGIN));
        return;
    }
    PlayerData data = this->persistence.loadPlayer(user.username);
    if (std::string(data.password) != user.password) {
        this->monitor.queueTheServerResponse(
                player_id, std::make_unique<ResponseLogin>(false, player_id, INVALID_PASSWORD));
        return;
    }
    Print::printMessageConsole("llega hasta aqui");
    this->loadingPlayerData(player_id, data);
    const std::string login_clan = this->clan_manager.getClanOf(data.username);
    if (!login_clan.empty()) {
        const std::string notif = std::string(data.username) + " entró al juego.";
        for (const auto& [id, p]: this->players) {
            if (id != player_id && this->clan_manager.areInSameClan(p->getName(), data.username)) {
                this->monitor.queueTheServerResponse(id, std::make_shared<ResponseChatMsg>(notif));
            }
        }
    }
    Print::printMessageConsole("llega hasta aqui");
    this->monitor.queueTheServerResponse(player_id,
                                         std::make_unique<ResponseLogin>(true, player_id));
    Map map = this->world.getMap();
    auto citizen_snapshot = ResponseBuilder::buildCitizenNpcSnapshot(this->citizen_npcs);
    this->monitor.queueTheServerResponse(
            player_id, std::make_unique<ResponseMap>(std::move(map), std::move(citizen_snapshot)));
    this->sendUpdateInventoryToPlayer(player_id, *this->players.at(player_id).get());
    this->sendUpdateEquipmentToPlayer(player_id, *this->players.at(player_id).get());
    // const auto inv = this->players.at(player_id)->getSlotsInventory();
    // MsgInventoryUpdate inv_msg{INVENTORY_UPDATE, inv};
    // this->sendResponseToPlayer(player_id, std::make_shared<ResponseInventoryUpdate>(inv_msg));
    const auto pending_it = this->pending_clan_msgs.find(std::string(data.username));
    if (pending_it != this->pending_clan_msgs.end()) {
        for (const auto& msg: pending_it->second) {
            this->monitor.queueTheServerResponse(player_id, std::make_shared<ResponseChatMsg>(msg));
        }
        this->pending_clan_msgs.erase(pending_it);
    }
}

void Gameloop::sendResponseToPlayer(Id player_id, std::shared_ptr<Response> response) {
    this->monitor.queueTheServerResponse(player_id, std::move(response));
}
void Gameloop::sendCombatMessage(Id target_id, const std::string& msg) {
    if (this->players.contains(target_id)) {
        this->sendResponseToPlayer(target_id, std::make_shared<ResponseChatMsg>(msg));
    }
}

void Gameloop::sendUpdateInventoryToPlayer(Id player_id, Player& player) {
    const auto inv = player.getSlotsInventory();
    MsgInventoryUpdate msg{INVENTORY_UPDATE, inv};
    this->sendResponseToPlayer(player_id, std::make_shared<ResponseInventoryUpdate>(msg));
}

void Gameloop::sendUpdateEquipmentToPlayer(Id player_id, Player& player) {
    const auto equip = player.getSlotsEquipment();
    MsgEquipmentUpdate msg{EQUIPMENT_UPDATE, equip};
    this->sendResponseToPlayer(player_id, std::make_shared<ResponseEquipmentUpdate>(msg));
}

void Gameloop::reportAttackByAPlayerOnClanmates(Player& player) {
    const auto victim_clan = this->clan_manager.getClanOf(player.getName());
    /*Con esto evitamos que nos quede la forma piramidal en el bloque de codigo*/
    if (victim_clan.empty()) {
        return;
    }
    const auto notif = GameMessageBuilder::mssgeFromAClanMemberUnderAttack(player.getName());
    for (const auto& [id, p]: this->players) {
        if (p->getName() != player.getName() &&
            this->clan_manager.areInSameClan(p->getName(), player.getName())) {
            this->monitor.queueTheServerResponse(id, std::make_shared<ResponseChatMsg>(notif));
        }
    }
}

void Gameloop::reportPlayerInterruptedMeditation(const Id& player_id, Player& player) {
    if (player.breakMeditation()) {
        const auto messg = GameMessageBuilder::messgInterruptedMeditation(player.getName());
        this->sendResponseToPlayer(player_id, std::make_shared<ResponseChatMsg>(messg));
    }
}

void Gameloop::updateAttackCooldown(const uint32_t& delta_ms) {
    for (auto& player: this->players | std::views::values) {
        player->updateAttackCooldown(delta_ms);
    }
}

bool Gameloop::isItPossibleToAttack(Player& player, const CombatEntity& victim, Weapon& weapon) {
    auto* magic_weapon = dynamic_cast<MagicWeapon*>(&weapon);
    if (magic_weapon && !player.hasEnoughMana(magic_weapon->mana_cost)) {
        return false;
    }
    // TODO: Review distanceBetweenTheAttackerAndTheVictim
    const Position& attacker_pos =
            player.getPosition();  // this->world.positionPlayerInTheWorld(player_id);
    const Position& victim_pos = victim.getPosition();
    int distance = std::abs(static_cast<int>(attacker_pos.x) - static_cast<int>(victim_pos.x)) +
                   std::abs(static_cast<int>(attacker_pos.y) - static_cast<int>(victim_pos.y));
    return distance <= static_cast<int>(weapon.range_attack);
}

CombatEntity* Gameloop::inSearchOfTheVictimAttack(const Id& id_search) const {
    if (this->players.contains(id_search)) {
        return this->players.at(id_search).get();
    }
    if (this->creatures.contains(id_search)) {
        return this->creatures.at(id_search).get();
    }
    return nullptr;
}

std::vector<Defense*> Gameloop::getPlayerDefensiveEquipment(const Id& player_id) {
    std::vector<Defense*> info_equipment_defensive;
    std::vector<TypeItem> equipmentTypes = this->players[player_id]->getEquipment();
    for (auto type: equipmentTypes) {
        const auto item = this->conf.items.find(type);
        if (item == this->conf.items.end()) {
            continue;
        }
        if (auto* defense = dynamic_cast<Defense*>(item->second.get())) {
            info_equipment_defensive.push_back(defense);
        }
    }
    return info_equipment_defensive;
}

void Gameloop::executeAttackPlayer(const Id& attacker_id, const Id& victim_id) {
    Print::printInitAttackPlayer(attacker_id, victim_id);
    Player* attacker = this->players.at(attacker_id).get();
    if (!attacker->isAlive()) {
        Print::printAttackPlayerIsDead();
        return;
    }
    if (!attacker->canAttack()) {
        const auto messg = GameMessageBuilder::messgStatingCannotUseWeapon(attacker->getName());
        this->sendCombatMessage(attacker_id, messg);
        return;
    }
    const TypeItem weapon_type = attacker->getHandItem();
    if (weapon_type == NONE) {
        return;
    }
    if (weapon_type == ELVEN_FLUTE) {
        const auto* flute = dynamic_cast<ObjectMagic*>(this->conf.items.at(weapon_type).get());
        if (!flute || !attacker->hasEnoughMana(flute->mana_cost)) {
            this->sendResponseToPlayer(
                    attacker_id, std::make_shared<ResponseChatMsg>("No tenés suficiente maná."));
            return;
        }
        attacker->restoreAllHp();
        attacker->consumeMana(flute->mana_cost);
        this->effects.emitSound(SoundEffectID::CURAR, attacker->getPosition());
        const Position position = attacker->getPosition();
        this->effects.emitVisual(VisualEffectID::BE_HEALED, position);
        attacker->breakMeditation();
        this->reportPlayerInterruptedMeditation(attacker_id, *attacker);
        attacker->resetAttackCooldown(this->conf.times.player_attack_cooldown);
        return;
    }
    if (attacker_id == victim_id)
        return;

    CombatEntity* victim = this->inSearchOfTheVictimAttack(victim_id);
    if (!victim || !victim->isAlive()) {
        return;
    }
    if (!attacker->isValidOpponent(dynamic_cast<Player*>(victim))) {
        return;
    }
    /*DEBERIA EVALUARSE EN EL PLAYER ?*/
    if (const auto* victim_player = dynamic_cast<Player*>(victim)) {
        if (this->clan_manager.areInSameClan(attacker->getName(), victim_player->getName())) {
            return;
        }
    }
    const auto weapon = dynamic_cast<Weapon*>(this->conf.items.at(weapon_type).get());
    if (!weapon) {
        return;
    }
    /*ESTOS DOS IF LOS PODEMOS COMBINAR EN UNO*/
    const Position attacker_pos =
            attacker->getPosition();  // this->world.positionPlayerInTheWorld(attacker_id);
    if (this->world.isSafeZONE(attacker_pos)) {
        return;
    }
    /*El dynamicast es redundante, getPosition es de la clase clase combatiente*/
    if (dynamic_cast<Player*>(victim) && this->world.isSafeZONE(victim->getPosition())) {
        return;
    }

    if (!this->isItPossibleToAttack(*attacker, *victim, *weapon)) {
        return;
    }

    bool is_critical = false;
    uint16_t damage_by_attacker = attacker->calculateDamage(is_critical, *weapon);
    if (!is_critical && victim->dodgeAttack()) {
        if (dynamic_cast<Player*>(victim)) {
            auto mssg =
                    GameMessageBuilder::messgAttackDodged(attacker->getName(), victim->getName());
            this->sendCombatMessage(victim_id, mssg);
        }
        auto mssg = GameMessageBuilder::messgTheOpponentDodgedTheAttack(victim->getName());
        this->sendCombatMessage(attacker_id, mssg);
        attacker->resetAttackCooldown(this->conf.times.player_attack_cooldown);
        Print::printEvasiveMessageAttack(victim->getName());
        return;
    }
    damage_by_attacker += this->calcClanProximityBonus(attacker->getName(), attacker_pos);

    if (const auto player = dynamic_cast<Player*>(victim)) {
        const std::vector<Defense*> equip_defensive = this->getPlayerDefensiveEquipment(victim_id);
        const auto defense_victim = player->calculateDefense(equip_defensive);
        damage_by_attacker =
                (damage_by_attacker > defense_victim) ? (damage_by_attacker - defense_victim) : 0;
        const auto clan_def_bonus =
                this->calcClanProximityBonus(player->getName(), victim->getPosition());
        damage_by_attacker =
                (damage_by_attacker > clan_def_bonus) ? (damage_by_attacker - clan_def_bonus) : 0;
    }

    this->effects.emitSound(attackSoundFor(weapon_type), attacker_pos);

    const Position victim_position = victim->getPosition();
    if (const auto weapon_visual = attackVisualFor(weapon_type); weapon_visual.has_value()) {
        this->effects.emitVisual(*weapon_visual, victim_position);
    }
    if (damage_by_attacker > 0) {
        this->effects.emitVisual(VisualEffectID::DMG, victim_position);
    }

    if (dynamic_cast<Player*>(victim)) {
        this->effects.emitVisual(VisualEffectID::BE_ATTACKED, victim_position, victim_id);
    }

    victim->receiveDamage(damage_by_attacker, this->world);
    attacker->earnExperiencePoints(victim, damage_by_attacker);

    const bool victim_died = !victim->isAlive();
    const bool victim_is_creature = dynamic_cast<Creature*>(victim) != nullptr;

    if (victim_died) {
        attacker->earnKillExp(victim);
        this->effects.emitVisual(VisualEffectID::DEATH, victim_position);
    }
    if (victim_died && !victim_is_creature)
        this->effects.emitSound(SoundEffectID::MUERTE_HOMBRE, victim->getPosition());

    if (victim_died && victim_is_creature) {
        this->effects.emitSound(SoundEffectID::DROP_ESPECIAL_NPC, victim->getPosition());
    }

    if (!victim_is_creature) {
        if (const auto victim_player = dynamic_cast<Player*>(victim)) {
            this->reportPlayerInterruptedMeditation(victim_id, *victim_player);
            this->reportAttackByAPlayerOnClanmates(*victim_player);
            auto messg = GameMessageBuilder::damageMessgToThePlayer(
                    attacker->getName(), damage_by_attacker, victim_died);
            this->sendCombatMessage(victim_id, messg);
            if (victim_died) {
                this->sendUpdateEquipmentToPlayer(victim_id, *victim_player);
                this->sendUpdateInventoryToPlayer(victim_id, *victim_player);
            }
        }
    }
    auto messg = GameMessageBuilder::messgAboutDamageDealtByThePlayer(
            victim->getName(), damage_by_attacker, victim_died);
    this->sendCombatMessage(attacker_id, messg);

    if (victim_died && victim_is_creature) {
        this->creatures.erase(victim_id);
    }

    const auto magic_weapon = dynamic_cast<MagicWeapon*>(weapon);
    if (magic_weapon)
        attacker->consumeMana(magic_weapon->mana_cost);

    attacker->resetAttackCooldown(this->conf.times.player_attack_cooldown);
    this->reportPlayerInterruptedMeditation(attacker_id, *attacker);
}

void Gameloop::processMovePlayer(Id player_id, Direction dir) {
    if (this->pending_resurrects.contains(player_id)) {
        return;
    }
    if (this->world.isWalkable(player_id, dir)) {
        if (this->players[player_id]->breakMeditation()) {
            this->sendResponseToPlayer(
                    player_id, std::make_shared<ResponseChatMsg>("Tu meditación es interrumpida."));
        }
        Pose new_pose = this->world.movePlayer(player_id, dir);
        this->players[player_id]->updatePose(std::move(new_pose));
        bool& use_second_step = this->next_step_is_second[player_id];
        this->effects.emitSound(use_second_step ? SoundEffectID::PASO_2 : SoundEffectID::PASO,
                                this->players[player_id]->getPosition());
        use_second_step = !use_second_step;
    }
    // else {
    //     std::cerr << "[MOVE] rejected player=" << player_id << " reason=not_walkable" <<
    //     std::endl;
    // }
}

void Gameloop::processPlayerDisconnet(Id player_id) {
    if (!this->players.contains(player_id)) {
        return;
    }
    const auto& player = this->players.at(player_id);
    const std::string logout_username = player->getName();
    const std::string logout_clan = this->clan_manager.getClanOf(logout_username);
    if (!logout_clan.empty()) {
        const std::string notif = logout_username + " se desconectó.";
        for (const auto& [id, p]: this->players) {
            if (id != player_id &&
                this->clan_manager.areInSameClan(p->getName(), logout_username)) {
                this->monitor.queueTheServerResponse(id, std::make_shared<ResponseChatMsg>(notif));
            }
        }
    }
    this->updateStatePlayer(*player);
    this->world.removePlayer(player_id);
    this->pending_resurrects.erase(player_id);
    this->next_step_is_second.erase(player_id);
    this->players.erase(player_id);
}

void Gameloop::processBuyItem(Id player_id, Id npc_id, TypeItem type_item) {
    const auto trader = dynamic_cast<TraderNPC*>(this->citizen_npcs.at(npc_id).get());
    if (!trader) {
        return;
    }
    Player* player = this->players.at(player_id).get();
    if (!player->isAlive()) {
        return;
    }
    try {
        trader->executeBuyItem(*player, type_item);
        this->sendUpdateInventoryToPlayer(player_id, *player);
        this->effects.emitSound(SoundEffectID::MONEDAS, player->getPosition());
        this->effects.emitRandomSound(603, 609, player->getPosition());
    } catch (const InvalidBuyException& e) {
        this->sendResponseToPlayer(player_id, std::make_shared<ResponseChatMsg>(e.what()));
    }
    player->breakMeditation();
}

void Gameloop::processSellItem(Id player_id, Id npc_id, TypeItem type_item) {
    const auto merchant = dynamic_cast<Merchant*>(this->citizen_npcs.at(npc_id).get());
    if (!merchant) {
        return;
    }
    Player* player = this->players.at(player_id).get();
    if (!player->isAlive()) {
        return;
    }
    try {
        merchant->executePlayerSellsItem(*player, type_item);
        this->sendUpdateInventoryToPlayer(player_id, *player);
        this->effects.emitSound(SoundEffectID::MONEDAS, player->getPosition());
    } catch (const InvalidSellException& e) {
        this->sendResponseToPlayer(player_id, std::make_shared<ResponseChatMsg>(e.what()));
    }
    player->breakMeditation();
}

void Gameloop::processPlayerPickUp(Id player_id) {
    Player* player = this->players.at(player_id).get();
    if (!player->isAlive()) {
        return;
    }
    if (this->world.playerTakeItemOnTheFloor(*player)) {
        this->sendUpdateInventoryToPlayer(player_id, *player);
    }
    player->breakMeditation();
}

void Gameloop::processPlayerDropItem(Id player_id, size_t index_slot) {
    Player* player = this->players.at(player_id).get();
    if (!player->isAlive()) {
        return;
    }
    if (player->dropItem(index_slot, this->world)) {
        this->sendUpdateInventoryToPlayer(player_id, *player);
    }
    player->breakMeditation();
}

void Gameloop::processPlayerEquipItem(Id player_id, size_t slot_id) {
    Player* player = this->players.at(player_id).get();
    if (!player->isAlive())
        return;
    const TypeItem previous_hand_item = player->getHandItem();
    bool update_inventory = false;
    if (player->useItem(slot_id)) {
        update_inventory = true;
        this->effects.emitSound(SoundEffectID::TOMAR_POCION, player->getPosition());
    }
    if (player->equipItem(slot_id) && !update_inventory) {
        update_inventory = true;
        this->sendUpdateEquipmentToPlayer(player_id, *player);
        if (player->getHandItem() != NONE && player->getHandItem() != previous_hand_item)
            this->effects.emitSound(SoundEffectID::EQUIPAR_ARMA, player->getPosition());
    }
    if (update_inventory) {
        this->sendUpdateInventoryToPlayer(player_id, *player);
    }
    player->breakMeditation();
}

void Gameloop::processPlayerUnequipItem(Id player_id, size_t slot_id) {
    Player* player = this->players.at(player_id).get();
    if (!player->isAlive())
        return;
    if (player->unequipItem(slot_id)) {
        this->sendUpdateEquipmentToPlayer(player_id, *player);
        this->sendUpdateInventoryToPlayer(player_id, *player);
    }
    player->breakMeditation();
}

void Gameloop::processPlayerWithdrawItem(Id player_id, Id npc_id, TypeItem type_item) {
    const auto banker = dynamic_cast<Banker*>(this->citizen_npcs.at(npc_id).get());
    if (!banker) {
        return;
    }
    Player* player = this->players.at(player_id).get();
    if (!player->isAlive()) {
        return;
    }
    if (banker->playerWithdrawItem(*player, type_item)) {
        this->sendUpdateInventoryToPlayer(player_id, *player);
        this->effects.emitSound(SoundEffectID::DEPOSITAR_RETIRAR_BOVEDA_2, player->getPosition());
    }
    player->breakMeditation();
}

void Gameloop::processPlayerDepositItem(Id player_id, Id npc_id, TypeItem type_item) {
    Player* player = this->players.at(player_id).get();
    if (!player->isAlive()) {
        return;
    }
    auto banker = dynamic_cast<Banker*>(this->citizen_npcs.at(npc_id).get());
    if (!banker) {
        return;
    }
    if (banker->playerDepositItem(*player, type_item)) {
        this->sendUpdateInventoryToPlayer(player_id, *player);
        this->sendResponseToPlayer(
                player_id, std::make_shared<ResponseChatMsg>("Objeto depositado correctamente."));
        this->effects.emitSound(SoundEffectID::DEPOSITAR_RETIRAR_BOVEDA, player->getPosition());
        this->effects.emitRandomSound(610, 613, player->getPosition());
    } else {
        this->sendResponseToPlayer(player_id, std::make_shared<ResponseChatMsg>(
                                                      "No tenés ese objeto para depositar."));
    }
    player->breakMeditation();
}

void Gameloop::processPlayerDepositGold(Id player_id, Id npc_id, uint32_t amount) {
    Player* player = this->players.at(player_id).get();
    if (!player->isAlive()) {
        return;
    }
    auto banker = dynamic_cast<Banker*>(this->citizen_npcs.at(npc_id).get());
    if (!banker) {
        return;
    }
    const uint32_t deposited = banker->playerDepositGold(*player, amount);
    if (deposited > 0) {
        this->sendResponseToPlayer(
                player_id, std::make_shared<ResponseChatMsg>(
                                   "Se depositó " + std::to_string(deposited) + " de oro."));
        this->sendUpdateInventoryToPlayer(player_id, *player);
        this->effects.emitSound(SoundEffectID::DEPOSITAR_RETIRAR_BOVEDA, player->getPosition());
        this->effects.emitRandomSound(610, 613, player->getPosition());
    }
    player->breakMeditation();
}

void Gameloop::processPlayerWithdrawGold(Id player_id, Id npc_id, uint32_t amount) {
    auto player = this->players.at(player_id).get();
    if (!player->isAlive()) {
        return;
    }
    auto banker = dynamic_cast<Banker*>(this->citizen_npcs.at(npc_id).get());
    if (!banker) {
        return;
    }
    const uint32_t withdrawn = banker->playerWithdrawGold(*player, amount);
    if (withdrawn > 0) {
        this->sendResponseToPlayer(player_id,
                                   std::make_shared<ResponseChatMsg>(
                                           "Se retiró " + std::to_string(withdrawn) + " de oro."));
        this->sendUpdateInventoryToPlayer(player_id, *player);
        this->effects.emitSound(SoundEffectID::DEPOSITAR_RETIRAR_BOVEDA_2, player->getPosition());
    } else {
        this->sendResponseToPlayer(
                player_id, std::make_shared<ResponseChatMsg>("No tienes oro para retirar."));
    }
    player->breakMeditation();
}

void Gameloop::processListItems(Id player_id, Id npc_id) {
    std::unique_ptr<Response> response;
    Player* player = this->players.at(player_id).get();
    if (!player->isAlive()) {
        return;
    }
    auto npc = this->citizen_npcs.at(npc_id).get();
    if (auto trader = dynamic_cast<TraderNPC*>(npc)) {
        auto list_items = trader->listItemsCatalog();
        response = std::make_unique<ResponseTraderCatalog>(std::move(list_items));
        this->effects.emitRandomSound(600, 602, player->getPosition());
    }
    if (auto banker = dynamic_cast<Banker*>(npc)) {
        auto list_items = banker->depositedItems(*player);
        const auto gold_deposited = banker->depositedGold(*player);
        response = std::make_unique<ResponseBankContent>(std::move(list_items), gold_deposited);
        this->effects.emitSound(SoundEffectID::BIENVENIDO_AL_BANCO, player->getPosition());
    }
    this->monitor.queueTheServerResponse(player_id, std::move(response));
    player->breakMeditation();
}

void Gameloop::processPlayerMeditate(Id player_id) {
    Player* player = this->players.at(player_id).get();
    if (!player->isAlive()) {
        return;
    }
    player->toggleMeditation();
    if (player->isMeditating()) {
        auto mssg = GameMessageBuilder::messageReportingMditationStatus(player->getName());
        auto respo = std::make_shared<ResponseChatMsg>(mssg);
        this->sendResponseToPlayer(player_id, std::move(respo));
    }
}

uint32_t Gameloop::calculateResurrectionDelayMs(const Position& from, const Position& to) const {
    const uint32_t distance = World::distanceBetweenPositions(from, to);
    return std::max(MIN_RESURRECTION_DELAY_MS, distance * RESURRECTION_MS_PER_TILE);
}

void Gameloop::resurrectPlayerAtHealer(Id player_id, Id healer_id) {
    Player* player = this->players.at(player_id).get();
    auto healer = dynamic_cast<Priest*>(this->citizen_npcs.at(healer_id).get());
    if (!healer) {
        return;
    }
    healer->resurrect(*player, this->world, player_id);
    this->sendResponseToPlayer(player_id,
                               std::make_shared<ResponseChatMsg>("Has resucitado en la ciudad."));

    Position position = player->getPosition();
    this->effects.emitSound(SoundEffectID::RESUCITAR, position);
    this->effects.emitRandomSound(PRIEST_DIALOG_FIRST, PRIEST_DIALOG_LAST, position);

    this->effects.emitVisual(VisualEffectID::BE_HEALED, position, player_id);
}

void Gameloop::processPlayerHeal(Id player_id) {
    Player* player = this->players.at(player_id).get();
    if (!player->isAlive()) {
        return;
    }
    NpcInstance healer = this->world.findNearestHealer(player->getPosition());
    auto priest = dynamic_cast<Priest*>(this->citizen_npcs.at(healer.id).get());
    if (!priest) {
        return;
    }
    priest->heal(*player);
    Position position = player->getPosition();
    this->effects.emitSound(SoundEffectID::CURAR, position);
    this->effects.emitRandomSound(PRIEST_DIALOG_FIRST, PRIEST_DIALOG_LAST, position);

    this->effects.emitVisual(VisualEffectID::BE_HEALED, position, player_id);
    this->sendResponseToPlayer(player_id, std::make_shared<ResponseChatMsg>(
                                                  "Debes seleccionar un sacerdote para curarte."));
}

void Gameloop::processPlayerResurrect(Id player_id, std::optional<Id> priest_id) {
    Player* player = this->players.at(player_id).get();
    if (player->isAlive() || player->isResurrecting()) {
        return;
    }
    if (this->pending_resurrects.contains(player_id)) {
        return;
    }

    if (priest_id.has_value()) {
        auto npc_it = this->citizen_npcs.find(*priest_id);
        if (npc_it == this->citizen_npcs.end()) {
            this->sendResponseToPlayer(player_id,
                                       std::make_shared<ResponseChatMsg>("No hay ningún NPC ahí."));
            return;
        }
        auto* priest = dynamic_cast<Priest*>(npc_it->second.get());
        if (!priest) {
            this->sendResponseToPlayer(
                    player_id, std::make_shared<ResponseChatMsg>("Ese NPC no puede resucitarte."));
            return;
        }
        const uint32_t distance =
                World::distanceBetweenPositions(player->getPosition(), priest->getPosition());
        if (distance > 3) {
            this->sendResponseToPlayer(player_id, std::make_shared<ResponseChatMsg>(
                                                          "Estás demasiado lejos del sacerdote."));
            return;
        }
        this->resurrectPlayerAtHealer(player_id, *priest_id);
        return;
    }

    NpcInstance healer = this->world.findNearestHealer(player->getPosition());
    uint32_t delay_ms =
            this->calculateResurrectionDelayMs(player->getPosition(), healer.pose.position);
    player->startResurrection();
    this->pending_resurrects[player_id] = {delay_ms, healer.id};
    this->sendResponseToPlayer(
            player_id, std::make_shared<ResponseChatMsg>("Buscando sacerdote para resucitarte..."));
}

void Gameloop::processPlayerInteract(Id player_id, Id npc_id, uint8_t action) {
    if (!this->players.contains(player_id)) {
        return;
    }
    Player* player = this->players.at(player_id).get();
    if (!player->isAlive() && action != 1) {
        return;
    }
    if (!this->citizen_npcs.contains(npc_id)) {
        this->sendResponseToPlayer(player_id,
                                   std::make_shared<ResponseChatMsg>("No hay ningún NPC ahí."));
        return;
    }
    auto priest = dynamic_cast<Priest*>(this->citizen_npcs.at(npc_id).get());
    if (!priest) {
        this->sendResponseToPlayer(player_id,
                                   std::make_shared<ResponseChatMsg>("Ese NPC no puede curarte."));
        return;
    }
    const uint32_t distance =
            World::distanceBetweenPositions(player->getPosition(), priest->getPosition());
    if (distance > 3) {
        this->sendResponseToPlayer(player_id, std::make_shared<ResponseChatMsg>(
                                                      "Estás demasiado lejos del sacerdote."));
        return;
    }
    if (action == 0) {
        priest->heal(*player);
        this->sendResponseToPlayer(player_id,
                                   std::make_shared<ResponseChatMsg>("Un sacerdote te cura."));
        Position position = player->getPosition();
        this->effects.emitSound(SoundEffectID::CURAR, position);
        this->effects.emitRandomSound(PRIEST_DIALOG_FIRST, PRIEST_DIALOG_LAST, position);
        this->effects.emitVisual(VisualEffectID::BE_HEALED, position, player_id);
    }
}

void Gameloop::processPlayerDebugKill(Id player_id) {
    Player* player = this->players.at(player_id).get();
    if (!player->isAlive()) {
        return;
    }
    player->receiveDamage(std::numeric_limits<uint16_t>::max(), this->world);
    this->effects.emitSound(SoundEffectID::MUERTE_HOMBRE, player->getPosition());
    const Position position = player->getPosition();
    this->effects.emitVisual(VisualEffectID::DEATH, position);
    this->sendUpdateEquipmentToPlayer(player_id, *player);
    this->sendUpdateInventoryToPlayer(player_id, *player);
}

void Gameloop::processPlayerToggleInfiniteHp(Id player_id) {
    Player* player = this->players.at(player_id).get();
    player->toggleInfiniteHp();
    if (player->hasInfiniteHp()) {
        player->restoreAllHp();
    }
}

void Gameloop::processPlayerToggleInfiniteMana(Id player_id) {
    Player* player = this->players.at(player_id).get();
    player->toggleInfiniteMana();
    if (player->hasInfiniteMana()) {
        player->restoreAllMana();
    }
}

void Gameloop::processBroadcastChat(Id sender_id, const std::string& text) {
    if (!this->players.contains(sender_id)) {
        return;
    }
    const std::string sender_name = this->players.at(sender_id)->getName();
    std::ostringstream oss;
    oss << sender_name << ": " << text;
    const std::string formatted = oss.str();
    for (auto& [id, player]: this->players) {
        this->sendResponseToPlayer(id, std::make_shared<ResponseChatMsg>(formatted));
    }
}

void Gameloop::processDirectChat(Id sender_id, Id target_id, const std::string& text) {
    if (!this->players.contains(sender_id) || !this->players.contains(target_id)) {
        return;
    }
    const std::string sender_name = this->players.at(sender_id)->getName();
    const std::string target_name = this->players.at(target_id)->getName();

    std::ostringstream oss_to_target;
    oss_to_target << "[De " << sender_name << "]: " << text;
    this->sendResponseToPlayer(target_id, std::make_shared<ResponseChatMsg>(oss_to_target.str()));

    std::ostringstream oss_to_sender;
    oss_to_sender << "[Para " << target_name << "]: " << text;
    this->sendResponseToPlayer(sender_id, std::make_shared<ResponseChatMsg>(oss_to_sender.str()));
}

void Gameloop::processDirectChatByName(Id sender_id, const std::string& target_name,
                                       const std::string& text) {
    if (!this->players.contains(sender_id)) {
        return;
    }
    for (auto& [id, player]: this->players) {
        if (player->getName() == target_name) {
            this->processDirectChat(sender_id, id, text);
            return;
        }
    }
}

void Gameloop::executeRequest() {
    std::unique_ptr<Command> cmd;
    while (this->commands_queue.try_pop(cmd)) {
        try {
            cmd->execute(*this);
        } catch (const std::exception& e) {
            std::cerr << "[Gameloop] execuetRequest() exited: " << e.what() << std::endl;
        }
    }
}

void Gameloop::executeBroacastSnapshot() {
    Snapshot snap;
    snap.players = ResponseBuilder::buildPlayerSnapshot(this->players);
    for (auto& player_snapshot: snap.players) {
        const auto pending = this->pending_resurrects.find(player_snapshot.id);
        if (pending != this->pending_resurrects.end()) {
            player_snapshot.resurrection_time_left_ms = static_cast<uint16_t>(std::min<uint32_t>(
                    pending->second.time_left_ms, std::numeric_limits<uint16_t>::max()));
        }
    }
    snap.npcs = ResponseBuilder::buildNpcSnapshot(this->creatures);
    snap.items_on_floor = this->world.itemsOnTheFloor();
    this->effects.flushInto(snap);
    RespSnapshot resp_snap = std::make_unique<ResponseSnapshot>(std::move(snap));
    this->monitor.executeBroadcast(std::move(resp_snap));
}


Player* Gameloop::findNearestPlayer(const Creature& creature, Id& player_id) {
    Player* nearest = nullptr;
    uint32_t nearest_distance = std::numeric_limits<uint32_t>::max();
    for (auto& [id, player]: this->players) {
        if (!player->isAlive() || this->world.isSafeZONE(player->getPosition()) ||
            !this->world.isPositionInCreatureZone(creature.getId(), player->getPosition())) {
            continue;
        }
        const uint32_t distance =
                this->world.distanceBetweenPositions(creature.getPosition(), player->getPosition());
        const uint32_t aggro_range =
                std::max<uint32_t>(creature.getAggroRange(), MIN_CREATURE_AGGRO_RANGE);
        if (distance <= aggro_range && distance < nearest_distance) {
            nearest = player.get();
            player_id = id;
            nearest_distance = distance;
        }
    }
    return nearest;
}

void Gameloop::moveCreatureTowards(Id creature_id, Creature& creature, const Position& target) {
    const Position current = creature.getPosition();
    const uint32_t dx = current.x > target.x ? current.x - target.x : target.x - current.x;
    const uint32_t dy = current.y > target.y ? current.y - target.y : target.y - current.y;

    Direction horizontal = target.x < current.x ? LEFT : RIGHT;
    Direction vertical = target.y < current.y ? UP : DOWN;
    Direction first = dx >= dy ? horizontal : vertical;
    Direction second = dx >= dy ? vertical : horizontal;

    if ((dx > 0 && first == horizontal) || (dy > 0 && first == vertical)) {
        if (this->world.isCreatureWalkable(creature_id, first)) {
            creature.updatePose(this->world.moveCreature(creature_id, first));
            creature.resetMovementCooldown(CREATURE_MOVEMENT_COOLDOWN_MS);
            return;
        }
    }
    if (((dx > 0 && second == horizontal) || (dy > 0 && second == vertical)) &&
        this->world.isCreatureWalkable(creature_id, second)) {
        creature.updatePose(this->world.moveCreature(creature_id, second));
        creature.resetMovementCooldown(CREATURE_MOVEMENT_COOLDOWN_MS);
    }
}

void Gameloop::executeCreatureAttack(Creature& creature, Id player_id) {
    Player* victim = this->players.at(player_id).get();
    if (!victim->isAlive() || this->world.isSafeZONE(victim->getPosition()) ||
        !this->world.isPositionInCreatureZone(creature.getId(), victim->getPosition())) {
        return;
    }

    bool is_critical = false;
    uint16_t damage = creature.calculateDamage(is_critical);
    if (!is_critical && victim->dodgeAttack()) {
        creature.resetAttackCooldown(this->conf.times.npc_attack_cooldown);
        creature.resetMovementCooldown(CREATURE_MOVEMENT_COOLDOWN_MS);
        return;
    }

    const std::vector<Defense*> equipment = this->getPlayerDefensiveEquipment(player_id);
    const uint16_t defense = victim->calculateDefense(equipment);
    damage = damage > defense ? damage - defense : 0;

    this->effects.emitSound(SoundEffectID::GOLPE_RECIBIDO, victim->getPosition());
    this->effects.emitVisual(VisualEffectID::BE_ATTACKED, victim->getPosition(), player_id);

    victim->receiveDamage(damage, this->world);
    victim->breakMeditation();
    if (damage > 0) {
        const Position position = victim->getPosition();
        this->effects.emitVisual(VisualEffectID::DMG, position);
    }
    if (!victim->isAlive()) {
        this->effects.emitSound(SoundEffectID::MUERTE_HOMBRE, victim->getPosition());
        const Position position = victim->getPosition();
        this->effects.emitVisual(VisualEffectID::DEATH, position);
        this->sendUpdateEquipmentToPlayer(player_id, *victim);
        this->sendUpdateInventoryToPlayer(player_id, *victim);
    }
    creature.resetAttackCooldown(this->conf.times.npc_attack_cooldown);
    creature.resetMovementCooldown(CREATURE_MOVEMENT_COOLDOWN_MS);
}

void Gameloop::updateCreatures(uint32_t delta_ms) {
    for (auto& [creature_id, creature]: this->creatures) {
        creature->updateCooldowns(delta_ms);
        if (!creature->isAlive())
            continue;

        Id target_id = 0;
        Player* target = this->findNearestPlayer(*creature, target_id);
        if (!target) {
            continue;
        }

        const auto distance = this->world.distanceBetweenPositions(creature->getPosition(),
                                                                   target->getPosition());
        if (distance == 1 && creature->canAttack()) {
            this->executeCreatureAttack(*creature, target_id);
        } else if (distance > 1 && creature->canMove()) {
            this->moveCreatureTowards(creature_id, *creature, target->getPosition());
        }
    }
}

void Gameloop::respawnDeadNpcs() {
    this->spawn.spawnCreaturesZones(this->next_npc_id, this->creatures);
    this->spawn.spawnTreasuresZones();
}

void Gameloop::updatePlayersAttributes() {
    const float delta = this->conf.times.update_player_atributes / 1000.0f;
    for (auto& player: this->players | std::views::values) {
        if (player->hasInfiniteHp()) {
            player->restoreAllHp();
        } else {
            player->updateHp(delta);
        }
        if (player->hasInfiniteMana()) {
            player->restoreAllMana();
        } else if (player->isMeditating()) {
            player->meditating(delta);
        } else {
            player->updateMana(delta);
        }
    }
}

void Gameloop::updatePendingResurrects(const uint32_t& delta_ms) {
    for (auto it = this->pending_resurrects.begin(); it != this->pending_resurrects.end();) {
        if (it->second.time_left_ms <= delta_ms) {
            Id p_id = it->first;
            Player* player = this->players.at(p_id).get();
            if (player) {
                this->resurrectPlayerAtHealer(p_id, it->second.healer_id);
            } else if (this->players.contains(p_id)) {
                this->players.at(p_id)->finishResurrection();
            }
            it = this->pending_resurrects.erase(it);
        } else {
            it->second.time_left_ms -= delta_ms;
            ++it;
        }
    }
}

void Gameloop::updateStatePlayer(Player& player) {
    PlayerData data = player.getPlayerData(this->bank);
    this->persistence.schedulePlayers(std::move(data));
}

void Gameloop::updateStateWorld() {
    WorldStateData world_state = this->world.buildWorldState();
    for (const auto& creature: this->creatures | std::views::values) {
        CreatureData data = creature->getCreatureData();
        world_state.creatures.push_back(data);
    }
    for (const auto& citizen: this->citizen_npcs | std::views::values) {
        CitizenNpcData data = citizen->getCitizenNPCData();
        world_state.citizen.emplace_back(data);
    }
    this->persistence.scheduleWorld(std::move(world_state));
}

void Gameloop::updateStatePlayers() {
    for (const auto& player: this->players | std::views::values) {
        this->updateStatePlayer(*player);
    }
}

void Gameloop::run() {
    std::cerr << "[Gameloop] run() started" << std::endl;
    try {
        this->persistence.start();
        uint32_t timer_attributes = 0;
        uint32_t timer_spawn_npcs = 0;
        uint32_t timer_persistence = 0;
        const uint32_t TICK_MS = 1000 / this->conf.times.server_update_frecuency;
        while (should_keep_running()) {
            this->executeRequest();
            this->updateCreatures(TICK_MS);
            this->updatePendingResurrects(TICK_MS);
            this->updateAttackCooldown(TICK_MS);
            timer_attributes += TICK_MS;
            if (timer_attributes >= this->conf.times.update_player_atributes) {
                this->updatePlayersAttributes();
                timer_attributes = 0;
            }
            timer_spawn_npcs += TICK_MS;
            if (timer_spawn_npcs >= this->conf.times.spawn_npcs) {
                this->respawnDeadNpcs();
                timer_spawn_npcs = 0;
            }
            timer_persistence += TICK_MS;
            if (timer_persistence >= this->conf.times.pesistence_data) {
                this->updateStatePlayers();
                this->updateStateWorld();
                timer_persistence = 0;
            }
            this->executeBroacastSnapshot();
            std::this_thread::sleep_for(std::chrono::milliseconds(TICK_MS));
        }
    } catch (const ClosedQueue&) {
        std::cerr << "[Gameloop] run() exited: ClosedQueue" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "[Gameloop] run() exited: " << e.what() << std::endl;
    }
    this->updateStatePlayers();
    this->updateStateWorld();
    this->persistence.stop();
    this->persistence.join();
}

void Gameloop::stop() {
    Thread::stop();
    this->persistence.stop();
}

// ---------------------------------------------------------------------------
// Helpers de clan
// ---------------------------------------------------------------------------

std::optional<Id> Gameloop::findPlayerIdByUsername(const std::string& username) const {
    for (const auto& [id, player]: this->players) {
        if (player->getName() == username)
            return id;
    }
    return std::nullopt;
}

void Gameloop::sendClanOpResult(Id caller_id, const ClanOpResult& result) {
    this->monitor.queueTheServerResponse(caller_id, std::make_shared<ResponseChatMsg>(result.msg));
    if (!result.target_nick.empty()) {
        auto target_id = this->findPlayerIdByUsername(result.target_nick);
        if (target_id.has_value()) {
            this->monitor.queueTheServerResponse(
                    *target_id, std::make_shared<ResponseChatMsg>(result.target_msg));
        } else {
            this->pending_clan_msgs[result.target_nick].push_back(result.target_msg);
        }
    }
}

uint16_t Gameloop::calcClanProximityBonus(const std::string& username, const Position& pos) const {
    std::map<std::string, Position> online_positions;
    for (const auto& [id, p]: this->players) {
        if (p->isAlive())
            online_positions[p->getName()] = p->getPosition();
    }
    const uint8_t nearby = this->clan_manager.countNearbyMembers(username, pos, online_positions,
                                                                 this->conf.clan.proximity_range);
    return static_cast<uint16_t>(nearby) * this->conf.clan.proximity_bonus_per_member;
}

// ---------------------------------------------------------------------------
// Comandos de clan
// ---------------------------------------------------------------------------

void Gameloop::processClanFound(Id player_id, const std::string& clan_name) {
    const std::string username = this->players.at(player_id)->getName();
    const uint8_t level = static_cast<uint8_t>(this->players.at(player_id)->getLevel());
    const bool had_clan = !this->clan_manager.getClanOf(username).empty();
    ClanOpResult result = this->clan_manager.foundClan(username, clan_name, level,
                                                       this->conf.clan.min_level_to_found);
    if (!had_clan && this->clan_manager.getClanOf(username) == clan_name)
        this->effects.emitSound(SoundEffectID::CREACION_CLAN,
                                this->players.at(player_id)->getPosition());
    this->sendClanOpResult(player_id, result);
}

void Gameloop::processClanJoin(Id player_id, const std::string& clan_name) {
    const std::string username = this->players.at(player_id)->getName();
    ClanOpResult result = this->clan_manager.requestJoin(username, clan_name);
    this->sendClanOpResult(player_id, result);
}

void Gameloop::processClanReview(Id player_id) {
    const std::string username = this->players.at(player_id)->getName();
    ClanOpResult result = this->clan_manager.reviewClan(username);
    std::istringstream ss(result.msg);
    std::string line;
    while (std::getline(ss, line)) {
        if (!line.empty())
            this->monitor.queueTheServerResponse(player_id,
                                                 std::make_shared<ResponseChatMsg>(line));
    }
}

void Gameloop::processClanAccept(Id player_id, const std::string& nick) {
    const std::string username = this->players.at(player_id)->getName();
    ClanOpResult result =
            this->clan_manager.acceptMember(username, nick, this->conf.clan.max_members);
    this->sendClanOpResult(player_id, result);
}

void Gameloop::processClanReject(Id player_id, const std::string& nick) {
    const std::string username = this->players.at(player_id)->getName();
    ClanOpResult result = this->clan_manager.rejectMember(username, nick);
    this->sendClanOpResult(player_id, result);
}

void Gameloop::processClanBan(Id player_id, const std::string& nick) {
    const std::string username = this->players.at(player_id)->getName();
    ClanOpResult result = this->clan_manager.banMember(username, nick);
    this->sendClanOpResult(player_id, result);
}

void Gameloop::processClanKick(Id player_id, const std::string& nick) {
    const std::string username = this->players.at(player_id)->getName();
    ClanOpResult result = this->clan_manager.kickMember(username, nick);
    this->sendClanOpResult(player_id, result);
}

void Gameloop::processClanLeave(Id player_id) {
    const std::string username = this->players.at(player_id)->getName();
    ClanOpResult result = this->clan_manager.leaveClan(username);
    this->sendClanOpResult(player_id, result);
}

void Gameloop::processClanList(Id player_id) {
    this->sendResponseToPlayer(player_id,
                               std::make_shared<ResponseChatMsg>("--- Clanes disponibles ---"));
    std::vector<std::string> clan_list = this->clan_manager.listClans();
    if (clan_list.empty()) {
        this->sendResponseToPlayer(
                player_id, std::make_shared<ResponseChatMsg>("No hay clanes fundados aun."));
    } else {
        for (const auto& clan_info: clan_list) {
            this->sendResponseToPlayer(player_id,
                                       std::make_shared<ResponseChatMsg>("- " + clan_info));
        }
    }
}

Gameloop::~Gameloop() { std::cerr << "[Gameloop] run() finished" << std::endl; }
