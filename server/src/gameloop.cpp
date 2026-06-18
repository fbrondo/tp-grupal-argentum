#include "server/includes/gameloop.h"

#include <iostream>
#include <memory>
#include <ranges>
#include <string>
#include <tuple>
#include <utility>

#include "common/includes/map/map.h"
#include "common/includes/map/tile.h"
#include "server/includes/commands/command.h"
#include "server/includes/commands/command_signup.h"
#include "server/includes/core/data.h"
#include "server/includes/entity/combat_entity.h"
#include "server/includes/npc/banker.h"
#include "server/includes/npc/creature.h"
#include "server/includes/npc/merchant.h"
#include "server/includes/npc/priest.h"
#include "server/includes/response_builder.h"
#include "server/includes/responses/response_bank_content.h"
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

using RespSnapshot = std::shared_ptr<ResponseSnapshot>;

Gameloop::Gameloop(GameConfig&& conf_, MonitorQueues& monitor, QueueCmd& cmmds_queue):
        monitor(monitor),
        commands_queue(cmmds_queue),
        conf(std::move(conf_)),
        world(this->conf.paths.map),
        persistence(this->conf.paths),
        spawn(this->conf, this->world) {

    if (persistence.worldStateExists()) {
        Print::printInitGameloop("CARGANDO WORLD");
        WorldStateData world_data = persistence.loadWorldState();
    } else {
        Print::printInitGameloop("INICIANDO WORLD");
        this->spawn.spawnCitizenNpcZones(this->next_npc_id, this->citizen_npcs, this->bank);
        this->spawn.spawnCreaturesZones(this->next_npc_id, this->creatures);
        // this->spawn.spawnTreasuresZones();
    }
    Print::printMessageConsole("FINAL CONSTRUCTOR GAMELOOP");
    // Print::printNpcsSafeLoads(this->conf.npcs);
    // Print::printCreatureLoads(this->conf.creatures);
    // Print::printRacesLoad(this->conf.races);
    // Print::printClasesLoad(this->conf.clases);
}
//
// void Gameloop::loadTreasures(const WorldStateData& world_data) {
//     for (const auto&[pos_x, pos_y]: world_data.treasures) {
//         Position pos(pos_x, pos_y);
//         this->treasurePlacemen(pos);
//     }
// }
//
// void Gameloop::loadCreatures(const WorldStateData& world_data) {
//     for (const auto& data: world_data.creatures) {
//         NpcAttributes attrib;
//         const auto type = static_cast<TypeNPC>(data.type);
//         attrib.hp_current = data.hp;
//         attrib.hp_max = data.max_hp;
//         attrib.difficulty_level = data.level;
//         attrib.range_attack = data.range_attack;
//         const auto dir = static_cast<Direction>(data.direction);
//         Pose pose(Position{data.x, data.y}, dir);
//         this->createCreature(type, std::move(pose), attrib);
//     }
// }
//
// void Gameloop::loadCitizenNPCs(const WorldStateData& world_data) {
//     for (const auto& data: world_data.creatures) {
//         const auto type = static_cast<TypeNPC>(data.type);
//         const auto dir = static_cast<Direction>(data.direction);
//         Pose pose(Position{data.x, data.y}, dir);
//         //this->createNpcCity(std::move(pose), type);
//     }
// }
//
// void Gameloop::loadGoldBags(const WorldStateData& world_data)  {
//     for (const auto& data:world_data.gold_bags) {
//         GoldBagInstance gold;
//         gold.amount = data.amount;
//         gold.pos = Position(data.pos_x, data.pos_x);
//         this->world.addGoldWorld(gold);
//
//     }
// }
//
// void Gameloop::loadItems(const WorldStateData& world_data) {
//     for (const auto& data: world_data.items) {
//         ItemInstance instance;
//         const auto type = static_cast<TypeItem>(data.type_item);
//         instance.item = this->conf.items[type].get();
//         instance.pos = Position(data.x, data.y);
//         this->world.addItemWorld(instance);
//     }
// }
//
// void Gameloop::loadWorld(const WorldStateData& world_data) {
//     this->loadCitizenNPCs(world_data);
//     this->loadCreatures(world_data);
//     this->loadTreasures(world_data);
//     this->loadGoldBags(world_data);
//     this->loadItems(world_data);
// }


Character Gameloop::createCharacter(const CharacterTraits& traits) const {
    const auto type_race = static_cast<TypeRace>(traits.race);
    const auto type_clase = static_cast<TypeClase>(traits.clase);
    const Race& race = this->conf.races.at(type_race);
    const Clase& clase = this->conf.clases.at(type_clase);
    return Character(race, clase, traits.head, traits.body);
}

// Inventory Gameloop::loadingInventory(const PlayerData& player) {
//     Inventory result;
//     result.golden = player.golden;
//     for (const auto& item_data: player.inventory) {
//         auto type = static_cast<TypeItem>(item_data.type_item);
//         auto item = dynamic_cast<ShopItem*>(this->conf.items.at(type).get());
//         auto item_instance = std::make_unique<ItemInstance>(item);
//        // result.inventory.emplace_back(std::move(item_instance));
//     }
//     return result;
// }

void Gameloop::loadingPlayerData(const Id& player_id, const PlayerData& player_data) {
    Character charact = this->createCharacter(player_data.charact_traits);
    Inventory inv = this->loadingInventory(player_data);
    Position position(player_data.x, player_data.y);
    if (!this->world.isFreePosition(position)) {
        position = this->world.findNearbyFreePosition(position);
        if (!this->world.isFreePosition(position)) {
            position = this->world.calculatePositionRandomSafeZone();
        }
    }
    Direction dir = static_cast<Direction>(player_data.direction);
    Pose pose(position, dir);
    auto new_player = std::make_unique<Player>(pose, std::move(inv), std::move(ch), player_data);
    this->players.emplace(player_id, std::move(new_player));
    this->world.addPlayerWorld(player_id, pose);
}

void Gameloop::createNewPlayer(const User& user, const CharacterTraits& traits) {

    Character ch = this->createCharacter(traits);
    const Position position_spawn = this->world.calculatePositionRandomSafeZone();
    Pose pose_spawn(position_spawn, DOWN);
    auto new_player =
            std::make_unique<Player>(User(user), pose_spawn, std::move(ch), this->conf.player_init);
    const PlayerData player_data = new_player->getPlayerData();
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
    this->loadingPlayerData(player_id, data);
    this->monitor.queueTheServerResponse(player_id,
                                         std::make_unique<ResponseLogin>(true, player_id));
    Map map = this->world.getMap();
    this->monitor.queueTheServerResponse(player_id, std::make_unique<ResponseMap>(std::move(map)));
}

void Gameloop::sendResponseToPlayer(Id player_id, std::shared_ptr<Response> response) {
    this->monitor.queueTheServerResponse(player_id, std::move(response));
}

bool Gameloop::isItPossibleToAttack(const Id& player_id, const Id& victim_id, Weapon& weapon) {
    auto magic_weapon = dynamic_cast<MagicWeapon*>(&weapon);
    if (magic_weapon && !this->players[player_id]->hasEnoughMana(magic_weapon->range_attack)) {
        return false;
    }
    uint16_t range = weapon.range_attack;
    int distance = this->world.distanceBetweenTheAttackerAndTheVictim(player_id, victim_id);
    if (distance > range) {
        return false;
    }
    return true;
}

CombatEntity* Gameloop::inSearchOfTheVictimAttack(const Id& id_search) const {
    CombatEntity* victim = nullptr;
    if (this->players.contains(id_search)) {
        victim = this->players.at(id_search).get();
    }
    if (this->creatures.contains(id_search)) {
        victim = this->creatures.at(id_search).get();
    }
    return victim;
}

std::vector<Defense*> Gameloop::getPlayerDefensiveEquipment(const Id& player_id) {
    std::vector<Defense*> info_equipment_defensive;
    std::vector<TypeItem> equipmentTypes = this->players[player_id]->getEquipment();
    for (auto Type: equipmentTypes) {
        auto* item_defensive = dynamic_cast<Defense*>(this->conf.items.at(Type).get());
        info_equipment_defensive.push_back(item_defensive);
    }
    return info_equipment_defensive;
}

void Gameloop::executeAttackPlayer(const Id& attacker_id, const Id& victim_id) {

    Player* attacker = this->players.at(attacker_id).get();
    if (!attacker->isAlive())
        return;

    const TypeItem weapon_type = attacker->getHandItem();
    if (weapon_type == NONE)
        return;

    CombatEntity* victim = this->inSearchOfTheVictimAttack(victim_id);
    if (!victim) /*Es un npc normal*/
        return;
    if (!attacker->isValidOpponent(dynamic_cast<Player*>(victim))) {
        return;
    }
    auto* weapon = dynamic_cast<Weapon*>(this->conf.items.at(weapon_type).get());
    if (!weapon)
        return;
    if (!this->isItPossibleToAttack(attacker_id, victim_id, *weapon)) {
        return;
    }
    bool is_critical = false;
    uint16_t damage_by_attacker = attacker->calculateDamage(is_critical, *weapon);
    if (!is_critical && victim->dodgeAttack()) {
        return;  // Registrar sonido de esquivado
    }
    if (const auto player = dynamic_cast<Player*>(victim)) { /*Si la victima es un jugador*/
        const std::vector<Defense*> equip_defensive = this->getPlayerDefensiveEquipment(victim_id);
        const uint16_t defense_victim = player->calculateDefense(equip_defensive);
        damage_by_attacker =
                (damage_by_attacker > defense_victim) ? (damage_by_attacker - defense_victim) : 0;
    }

    SoundEffectSnapshotData golpe_sound{};
    golpe_sound.effect_id = SoundEffectID::GOLPE_ARMA;
    Position position = attacker->getPosition();
    golpe_sound.pos_x = position.x;
    golpe_sound.pos_y = position.y;
    this->sounds_of_current_tick.push_back(std::move(golpe_sound));
    victim->receiveDamage(damage_by_attacker, this->world);
    if (!victim->isAlive() && dynamic_cast<Creature*>(victim)) {
        this->creatures.erase(victim_id);
    }
    this->players[attacker_id]->breakMeditation();
}

void Gameloop::processMovePlayer(Id player_id, Direction dir) {
    if (!this->players.contains(player_id)) {
        return;
    }
    if (this->world.isWalkable(player_id, dir)) {
        this->players[player_id]->breakMeditation();
        Pose new_pose = this->world.movePlayer(player_id, dir);
        this->players[player_id]->updatePose(std::move(new_pose));
    } else {
        std::cerr << "[MOVE] rejected player=" << player_id << " reason=not_walkable" << std::endl;
    }
}

void Gameloop::processBuyItem(Id player_id, Id npc_id, TypeItem type_item) {
    auto trader = dynamic_cast<TraderNPC*>(this->citizen_npcs.at(npc_id).get());
    if (!trader) {  // Enviar error como "Este NPC no vende ni compra nada."
        return;
    }
    Player* player = this->players.at(player_id).get();
    if (!player->isAlive()) {
        return;
    }
    trader->executeBuyItem(
            *player, type_item); /*Aca se puede tirar una excepcion, en caso de no comprar el item*/
    player->breakMeditation();
    // MsgInventoryUpdate msg{INVENTORY_UPDATE, slot, static_cast<uint16_t>(tipo_buscado), 1, 0};
    // this->sendResponseToPlayer(player_id, std::make_shared<ResponseInventoryUpdate>(msg));
}

void Gameloop::processSellItem(Id player_id, Id npc_id, TypeItem type_item) {
    const auto merchant = dynamic_cast<Merchant*>(this->citizen_npcs.at(npc_id).get());
    if (!merchant) {  // Enviar error como "Este NPC no compra nada."
        return;
    }
    Player* player = this->players.at(player_id).get();
    if (!player->isAlive()) {
        return;
    }
    merchant->executePlayerSellsItem(*player, type_item);
    player->breakMeditation();
}

void Gameloop::processPlayerPickUp(Id player_id) {
    Player* player = this->players.at(player_id).get();
    if (!player->isAlive()) {
        return;
    }
    this->world.playerTakeItemOnTheFloor(*player);
    player->breakMeditation();
    // MsgInventoryUpdate msg{INVENTORY_UPDATE, slot, item_type_id, 1, 0};
    // this->sendResponseToPlayer(player_id, std::make_shared<ResponseInventoryUpdate>(msg));
}

void Gameloop::processPlayerDropItem(Id player_id, size_t index_slot) {
    Player* player = this->players.at(player_id).get();
    if (!player->isAlive()) {
        return;
    }
    player->dropItem(index_slot, this->world);
    player->breakMeditation();
    // MsgInventoryUpdate msg{INVENTORY_UPDATE, slot, 0, 0, 0};
    // this->sendResponseToPlayer(player_id, std::make_shared<ResponseInventoryUpdate>(msg));
}

void Gameloop::processPlayerWithdrawItem(Id player_id, Id npc_id, TypeItem type_item) {
    const auto banker = dynamic_cast<Banker*>(this->citizen_npcs.at(npc_id).get());
    if (!banker) {  // Enviar error como "Este NPC no compra nada."
        return;
    }
    Player* player = this->players.at(player_id).get();
    if (!player->isAlive()) {
        return;
    }
    banker->playerWithdrawItem(*player, type_item);
    player->breakMeditation();
    // MsgInventoryUpdate msg{INVENTORY_UPDATE, slot, item_type_id, 1, 0};
    // this->sendResponseToPlayer(player_id, std::make_shared<ResponseInventoryUpdate>(msg));
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
    const auto item = dynamic_cast<const ShopItem*>(this->conf.items[type_item].get());
    banker->playerDepositItem(*player, item);
    // if (player->getBankSize() >= player->getMaxBankSize()) {
    //     // Error: "No tenes espacio en el banco para depositar esto."
    //     return;
    // }
    // uint8_t slot = player->getSlotOfInstance(instance_id);
    // std::unique_ptr<ItemInstance> item_to_deposit = player->removeItemFromInventory(instance_id);
    //
    // player->addItemToBank(std::move(item_to_deposit));
    player->breakMeditation();

    // MsgInventoryUpdate msg{INVENTORY_UPDATE, slot, 0, 0, 0};
    // this->sendResponseToPlayer(player_id, std::make_shared<ResponseInventoryUpdate>(msg));
}

void Gameloop::processPlayerDepositGold(Id player_id, uint32_t /*amount*/) {
    Player* player = this->players.at(player_id).get();
    if (!player->isAlive()) {
        return;
    }
    player->breakMeditation();
}

void Gameloop::processPlayerWithdrawGold(Id player_id, uint32_t /*amount*/) {
    auto player = this->players.at(player_id).get();
    if (!player->isAlive()) {
        return;
    }
    player->breakMeditation();
}

void Gameloop::processListItems(Id player_id, Id /*npc_id*/) {
    Player* player = this->players.at(player_id).get();
    if (!player)
        return;
    // CitizenNPC* npc = this->citizen_npcs.at(npc_id);
    // if (!npc)
    //     return;
    // // TypeNPC npc_type = npc->getTypeNPC();
    // //  NPC* npc_generico = this->npcs.at(npc_type).get();
    //
    // InteractionResult result = npc->interact();
    // if (result.type == InteractionType::TRADER_SHOP) {
    //     // auto response = std::make_unique<ResponseTraderCatalog>(*(result.trader_store));
    //     // this->sendResponseToPlayer(player_id, std::move(response));
    // } else if (result.type == InteractionType::BANK_BOX) {
    //     std::vector<MsgItemInfo> items_info = player->getBankItemsInfo();
    //     uint32_t bank_gold = player->getBankGold();
    //     auto response = std::make_unique<ResponseBankContent>(std::move(items_info), bank_gold);
    //     this->sendResponseToPlayer(player_id, std::move(response));
    // }
}

void Gameloop::processPlayerMeditate(Id player_id) {
    // En el update del gameloop donde se manejan los ticks se debe manejar la regeneracion de mana
    // cuando medita
    Player* player = this->players.at(player_id).get();
    if (!player->isAlive()) {
        return;
    }
    player->toggleMeditation();
}

// void Gameloop::processPlayerHeal(Id player_id) {
//     Player* player = this->players.at(player_id).get();
//     if (!player->isAlive()) {
//         return;
//     }
//     player->restoreAllHp();
//     player->restoreAllMana();
//
//     SoundEffectSnapshotData sound_effect;
//     sound_effect.effect_id = SoundEffectID::CURAR;
//     Position position = player->getPosition();
//     sound_effect.pos_x = position.x;
//     sound_effect.pos_y = position.y;
//     this->sounds_of_current_tick.push_back(std::move(sound_effect));
// }
//
// void Gameloop::processPlayerEquipItem(Id player_id, Id instance_id) {
//     Player* player = this->players.at(player_id).get();
//     if (!player || !player->isAlive())
//         return;
//     player->equipItem(instance_id);
//
//     uint8_t slot = player->getSlotOfInstance(instance_id);
//     uint16_t item_type_id = static_cast<uint16_t>(player->getItemInstance(instance_id)->type);
//     MsgInventoryUpdate msg{INVENTORY_UPDATE, slot, item_type_id, 1, 1};
//     this->sendResponseToPlayer(player_id, std::make_shared<ResponseInventoryUpdate>(msg));
// }
//
// void Gameloop::processPlayerUnequipItem(Id player_id, Id instance_id) {
//     Player* player = this->players.at(player_id).get();
//     if (!player || !player->isAlive())
//         return;
//     player->unequipItem(instance_id);
//
//     uint8_t slot = player->getSlotOfInstance(instance_id);
//     uint16_t item_type_id = static_cast<uint16_t>(player->getItemInstance(instance_id)->type);
//
//     MsgInventoryUpdate msg{INVENTORY_UPDATE, slot, item_type_id, 1, 0};
//     this->sendResponseToPlayer(player_id, std::make_shared<ResponseInventoryUpdate>(msg));
// }
//
// void Gameloop::processPlayerUseItem(Id player_id, Id instance_id) {
//     Player* player = this->players.at(player_id).get();
//     if (!player || !player->isAlive())
//         return;
//     uint8_t slot = player->getSlotOfInstance(instance_id);
//     player->useItem(instance_id);
//     MsgInventoryUpdate msg{INVENTORY_UPDATE, slot, 0, 0, 0};
//     this->sendResponseToPlayer(player_id, std::make_shared<ResponseInventoryUpdate>(msg));
// }
//
// void Gameloop::processPlayerResurrect(Id player_id) {
//     Player* player = this->players.at(player_id).get();
//     if (!player || player->isAlive())
//         return;
//
//     if (this->pending_resurrects.count(player_id) > 0)
//         return;
//
//     Position healer_pos = this->world.findNearbyHealerPosition(player->getPose().position);
//
//     this->pending_resurrects[player_id] = {5000, healer_pos};
// }


void Gameloop::execuetRequest() {
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
    snap.npcs = ResponseBuilder::buildNpcSnapshot(this->creatures);
    RespSnapshot resp_snap = std::make_unique<ResponseSnapshot>(std::move(snap));
    this->monitor.executeBroadcast(std::move(resp_snap));
}

void Gameloop::respawnDeadNpcs() {
    this->spawn.spawnCreaturesZones(this->next_npc_id, this->creatures);
    this->spawn.spawnTreasuresZones();
}
void Gameloop::updatePlayersAttributes() {
    const float delta = this->conf.times.update_player_atributes / 1000.0f;
    for (auto& player: this->players | std::views::values) {
        player->updateHp(delta);
        if (player->isMeditating()) {
            player->meditating(delta);
        } else {
            player->updateMana(delta);
        }
    }
}

void Gameloop::updateStateWorld() {
    WorldStateData world_state = this->world.buildWorldState();
    for (const auto& creature: this->creatures | std::views::values) {
        CreatureData data = creature->getCreatureData();
        world_state.creatures.push_back(data);
    }
    this->persistence.scheduleWorld(std::move(world_state));
}

void Gameloop::updateStatePlayers() {
    for (auto& [id, player]: this->players) {
        PlayerData data = player->getPlayerData();
        this->persistence.schedulePlayers(std::move(data));
    }
}


void Gameloop::run() {
    std::cerr << "[Gameloop] run() started" << std::endl;
    try {
        this->persistence.start();
        uint32_t timer_attributes = 0;
        uint32_t timer_spawn_npcs = 0;
        uint32_t timer_drop_items = 0;
        uint32_t timer_persistence = 0;
        const uint32_t TICK_MS = 1000 / this->conf.times.server_update_frecuency;
        while (should_keep_running()) {
            this->execuetRequest();
            for (auto it = this->pending_resurrects.begin();
                 it != this->pending_resurrects.end();) {
                if (it->second.time_left_ms <= TICK_MS) {
                    Id p_id = it->first;
                    Player* player = this->players.at(p_id).get();

                    if (player) {
                        player->teleportTo(it->second.healer_pos);
                        player->restoreAllHp();
                        player->restoreAllMana();
                    }
                    it = this->pending_resurrects.erase(it);
                } else {
                    it->second.time_left_ms -= TICK_MS;
                    ++it;
                }
            }
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
            timer_drop_items += TICK_MS;
            if (timer_drop_items >= this->conf.times.disappear_dropped_item) {
                // this->removeExpiredItems();
                timer_drop_items = 0;
            }
            timer_persistence += TICK_MS;
            if (timer_persistence >= this->conf.times.pesistence_data) {
                this->updateStatePlayers();
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
    this->persistence.join();
    std::cerr << "[Gameloop] run() finished" << std::endl;
}

void Gameloop::stop() {
    Thread::stop();  // Cambia el should_keep_running del Gameloop a false
    this->persistence
            .stop();  // <-- IMPORTANTE: Cambia el should_keep_running de la persistencia a false
}
Gameloop::~Gameloop() {}
