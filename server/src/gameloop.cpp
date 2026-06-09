#include "server/includes/gameloop.h"

#include <iostream>
#include <memory>
#include <ranges>
#include <string>
#include <tuple>
#include <utility>
#include "server/includes/entity/combat_entity.h"
#include "common/includes/map/tile.h"
#include "server/includes/commands/command.h"
#include "server/includes/commands/command_signup.h"
#include "server/includes/core/data.h"
#include "server/includes/npc/banker.h"
#include "server/includes/npc/creature.h"
#include "server/includes/npc/merchant.h"
#include "server/includes/npc/priest.h"
#include "server/includes/responses/response_bank_content.h"
#include "server/includes/responses/response_login.h"
#include "server/includes/response_builder.h"
#include "server/includes/responses/response_map.h"
#include "server/includes/responses/response_signup.h"
#include "server/includes/responses/response_snapshot.h"
#include "common/includes/map/map.h"
#include "server/print.h"

#define INVALID_REGISTER "Username already taken."
#define INVALID_LOGIN "El usuario o la contraseña son incorrectos."
#define INVALID_PASSWORD "Wrong password."

using RespSnapshot = std::shared_ptr<ResponseSnapshot>;

Gameloop::Gameloop(GameConfig&& conf_, MonitorQueues& monitor, QueueCmd& cmmds_queue):
        gen(std::random_device{}()), monitor(monitor), commands_queue(cmmds_queue), conf(std::move(conf_)),
        world(this->conf.paths.map, this->next_item_id),
        persistence(this->conf.paths) {
    this->initConfigurationCitizenNPC();
    if (persistence.worldStateExists()) {
        Print::printInitGameloop("CARGANDO WORLD");
        WorldStateData world_data = persistence.loadWorldState();

    } else {
        Print::printInitGameloop("INICIANDO WORLD");
        this->initWorld();
    }
    // Print::printNpcsSafeLoads(this->conf.npcs);
    // Print::printCreatureLoads(this->conf.creatures);
    // Print::printRacesLoad(this->conf.races);
    // Print::printClasesLoad(this->conf.clases);
}
void Gameloop::initConfigurationCitizenNPC() {
    NpcSafeZone banker_conf = conf.npcs["Banquero"];
    this->banker = std::make_unique<Banker>(banker_conf.type, banker_conf.name);
    NpcSafeZone merch_conf = conf.npcs["Comerciante"];
    this->merchant = std::make_unique<Merchant>(merch_conf.type, merch_conf.name, merch_conf.ids_items_store);
    NpcSafeZone priest_conf = conf.npcs["Sacerdote"];
    this->priest = std::make_unique<Priest>(priest_conf.type, priest_conf.name,priest_conf.ids_items_store);
}


void Gameloop::loadTreasures(const WorldStateData& world_data) {
    for (const auto&[pos_x, pos_y]: world_data.treasures) {
        Position pos(pos_x, pos_y);
        this->treasurePlacemen(pos);
    }
}

void Gameloop::loadCreatures(const WorldStateData& world_data) {
    for (const auto& data: world_data.creatures) {
        NpcAttributes attrib;
        const auto type = static_cast<TypeNPC>(data.type);
        attrib.hp_current = data.hp;
        attrib.hp_max = data.max_hp;
        attrib.difficulty_level = data.level;
        attrib.range_attack = data.range_attack;
        const auto dir = static_cast<Direction>(data.direction);
        Pose pose(Position{data.x, data.y}, dir);
        this->createCreature(type, std::move(pose), attrib);
    }
}

void Gameloop::loadCitizenNPCs(const WorldStateData& world_data) {
    for (const auto& data: world_data.creatures) {
        const auto type = static_cast<TypeNPC>(data.type);
        const auto dir = static_cast<Direction>(data.direction);
        Pose pose(Position{data.x, data.y}, dir);
        this->createNpcCity(std::move(pose), type);
    }
}

void Gameloop::loadGoldBags(const WorldStateData& world_data) {
    for (const auto& data:world_data.gold_bags) {
        GoldBagInstance gold;
        const Id& id = this->next_item_id++;
        gold.amount = data.amount;
        gold.pos = Position(data.pos_x, data.pos_x);
        this->world.addGoldWorld(id, gold);
        this->items.emplace(id, GOLD);
        Print::printPositionGold(id, gold);
    }
}

void Gameloop::loadItems(const WorldStateData& world_data) {
    for (const auto& data: world_data.items) {
        ItemInstance item;
        item.id = this->next_item_id++;
        item.type = static_cast<TypeItem>(data.type_item);
        const auto item_temp = dynamic_cast<ShopItem*>(this->conf.items[item.type].get());
        item.body_part_use = item_temp->body_part_use;
        item.classification = item_temp->classif;
        item.pos = Position(data.x, data.y);
        this->world.addItemWorld(item);
        this->items.emplace(item.id, item.type);
        Print::printPositionItem(item);
    }
}

std::vector<ItemInstance> Gameloop::items_drop_creature() {
    std::vector<ItemInstance> items_to_drop;
    for (const auto& [type_item, item]: this->conf.items) {
        auto item_temp = dynamic_cast<ShopItem*>(item.get());
        ItemInstance item_inst;
        item_inst.type = type_item;
        item_inst.classification = item_temp->classif;
        item_inst.body_part_use = item_temp->body_part_use;
        items_to_drop.push_back(item_inst);
    }
    return items_to_drop;
}

void Gameloop::initCreatures(Region type_region, const Id& zone_id) {
    const auto region = dynamic_cast<WildRegion*>(this->conf.regions[type_region].get());
    std::uniform_int_distribution<size_t> distrib_npc(0, region->npc_types.size() - 1);
    for (uint16_t i = 0; i < region->max_creatures; i++) {
        std::string name_npc = region->npc_types[distrib_npc(this->gen)];;
        const TypeNPC type_npc = this->conf.creatures[name_npc].type;
        const Position position_spawn = this->world.calculatePositionRandom(zone_id);
        Pose pose_spawn(position_spawn, DOWN);
        NpcAttributes attrib = this->creatingAttributesToCreature(name_npc);
        this->createCreature(type_npc, std::move(pose_spawn), attrib);
    }
    if (region->min_treasure.has_value() && region->max_treasure.has_value()) {
        const uint16_t min = region->min_treasure.value();
        const uint16_t max = region->max_treasure.value();
        std::uniform_int_distribution<uint16_t> distrib_treasure(min, max);
        const uint16_t number_treasure = distrib_treasure(this->gen);
        for (uint16_t i = 0; i < number_treasure; i++) {
            Position treasure_position = this->world.calculatePositionRandom(zone_id);
            this->treasurePlacemen(treasure_position);
        }
    }
}

void Gameloop::initNpcSafeZone(Region type_region, const Id& zona_id) {
    const auto region = dynamic_cast<SafeRegion*>(conf.regions[type_region].get());
    for (size_t index = 0; const auto& name_npc: region->npc_types) {
        const uint16_t numbers_npcs_region = region->numbers_npc[index];
        for (uint16_t i = 0; i < numbers_npcs_region; i++) {
            const Position pos_spawn = this->world.calculatePositionRandom(zona_id);
            Pose pose_spawn(pos_spawn, DOWN);
            TypeNPC type = this->conf.npcs[name_npc].type;
            Print::print_message_console(name_npc);
            this->createNpcCity(std::move(pose_spawn), type);
        }
        index++;
    }
}

void Gameloop::initNPCS() {
    const std::vector<std::tuple<Id, Region>> info_zones = world.getZones();
    for (auto [id, type_region]: info_zones) {
        if (conf.regions[type_region]->is_safe) {
            this->initNpcSafeZone(type_region, id);
        } else {
            this->initCreatures(type_region, id);
        }
    }
}

void Gameloop::initWorld() {
    this->initNPCS();
}

void Gameloop::loadWorld(const WorldStateData& world_data) {
    this->loadCitizenNPCs(world_data);
    this->loadCreatures(world_data);
    this->loadTreasures(world_data);
    this->loadGoldBags(world_data);
    this->loadItems(world_data);
}

void Gameloop::treasurePlacemen(const Position& position) {
    Id treasure_id = this->next_item_id++;
    this->items.emplace(treasure_id, TREASURE);
    this->world.addTreasuresWorld(treasure_id, position);
    Print::printPositionTreasure(treasure_id, position);
}

NpcAttributes Gameloop::creatingAttributesToCreature(const std::string& name_npc) {
    const CreatureConfig& creat = this->conf.creatures[name_npc];
    NpcAttributes attrib;
    attrib.hp_max = creat.hp_max_initial;
    attrib.hp_current = creat.hp_max_initial;
    attrib.range_attack = creat.attack_range;
    std::uniform_int_distribution<uint16_t> distrib_level(creat.minimal_level, creat.maximun_level);
    attrib.difficulty_level = distrib_level(this->gen);
    return attrib;
}

void Gameloop::createCreature(TypeNPC type, Pose&& pose, const NpcAttributes& attrib) {
    Id creature_id = this->next_creature_id++;
    std::vector<ItemInstance> items_ = this->items_drop_creature();
    auto creature = std::make_unique<Creature>(creature_id, type, pose, attrib, items_);
    this->creatures.emplace(creature_id, std::move(creature));
    this->world.addCreatureWorld(creature_id, type, pose);
    Print::printPositionCreature(creature_id, type, pose.position, attrib);
}

void Gameloop::createNpcCity(Pose&& pose, TypeNPC type) {
    CitizenNPC* new_npc;
    if (type == PRIEST) {
        new_npc = this->priest.get();
    } else if (type == MERCHANT) {
        new_npc = this->merchant.get();
    } else {
        new_npc = this->banker.get();
    }
    Id npc_id = this->next_npc_id++;
    this->npcs.emplace(npc_id, new_npc);
    this->world.addNpcWorld(npc_id, type, pose);
    Print::printPositionNPC(npc_id, type, pose.position);
}

Character Gameloop::createCharacter(const CharacterTraits& traits) const {
    const auto type_race = static_cast<TypeRace>(traits.race);
    const auto type_clase = static_cast<TypeClase>(traits.clase);
    const Race& race = this->conf.races.at(type_race);
    const Clase& clase = this->conf.clases.at(type_clase);
    return Character(race, clase, traits.head, traits.body);
}

Inventory Gameloop::loadingInventory(const PlayerData& player) {
    Inventory result;
    result.golden = player.golden;
    for (const auto& item_data: player.inventory) {
        auto type = static_cast<TypeItem>(item_data.type_item);
        const auto item = dynamic_cast<ShopItem*>(this->conf.items.at(type).get());
        ItemClassification classif = item->classif;
        BodyPart body_part = item->body_part_use;
        const Id& id_item = this->next_item_id++;
        auto item_instance = std::make_unique<ItemInstance>(id_item, type, classif, body_part);
        result.inventory.emplace(id_item, std::move(item_instance));
    }
    return result;
}

void Gameloop::loadingPlayerData(const Id& player_id, const PlayerData& player_data) {
    Character charact = this->createCharacter(player_data.charact_traits);
    Inventory inv = this->loadingInventory(player_data);
    Position position(player_data.x, player_data.y);
    Direction dir = static_cast<Direction>(player_data.direction);
    Pose pose(position, dir);
    this->world.addPlayerWorld(player_id, pose);
}

void Gameloop::createNewPlayer(const Id& player_id, const User& user, const CharacterTraits& traits) {
    Character ch = this->createCharacter(traits);
    const Position position_spawn = this->world.calculatePositionRandomSafeZone();
    Pose pose_spawn(position_spawn, DOWN);
    auto new_player =
            std::make_unique<Player>(User(user), pose_spawn, std::move(ch), this->conf.player_init);
    const PlayerData player_data = new_player->getPlayerData();
    this->persistence.savePlayer(player_data);
    this->players.emplace(player_id, std::move(new_player));
    world.addPlayerWorld(player_id, pose_spawn);
}

void Gameloop::processHandleSignup(const Id& player_id, const User& user, const CharacterTraits& traits) {
    Print::printNewPlayerArrived(player_id, user, static_cast<TypeRace>(traits.race), static_cast<TypeClase>(traits.clase));
    if (this->persistence.exists(user.username)) {
        this->monitor.queueTheServerResponse(
                player_id, std::make_unique<ResponseSignup>(false, INVALID_REGISTER));
        return;
    }
    this->createNewPlayer(player_id, user, traits);
    this->monitor.queueTheServerResponse(player_id, std::make_unique<ResponseSignup>(true));
    //this->executeBroacastSnapshot();
}

void Gameloop::processHandleLogin(const Id& player_id, const User& user) {
    if (!this->persistence.exists(user.username)) {
        this->monitor.queueTheServerResponse(player_id,std::make_unique<ResponseLogin>(false, INVALID_LOGIN));
        return;
    }
    PlayerData data = this->persistence.loadPlayer(user.username);
    if (std::string(data.password) != user.password) {
        this->monitor.queueTheServerResponse(player_id, std::make_unique<ResponseLogin>(false, INVALID_PASSWORD));
    }
    Print::printMessageConsole("JUGADOR CONECTADO");
    this->monitor.queueTheServerResponse(player_id, std::make_unique<ResponseLogin>(true));
    Map map = this->world.getMap();

    this->monitor.queueTheServerResponse(player_id, std::make_unique<ResponseMap>(std::move(map)));

}

void Gameloop::sendResponseToPlayer(Id player_id, std::shared_ptr<Response> response) {
    this->monitor.queueTheServerResponse(player_id, std::move(response));
}

bool Gameloop::isItPossibleToAttack(const Id& player_id, const Id& victim_id, Weapon& weapon) {
    MagicWeapon* magic_weapon = dynamic_cast<MagicWeapon*>(&weapon);
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
    if (this->npcs.contains(id_search)) {
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
    victim->receiveDamage(damage_by_attacker, this->world);
    if (!victim->isAlive() && dynamic_cast<Creature*>(victim)) {
        this->creatures.erase(victim_id);
    }
    this->players[attacker_id]->breakMeditation();
}

void Gameloop::processMovePlayer(Id player_id, Direction dir) {
    this->players[player_id]->breakMeditation();
    Pose new_pose = this->world.movePlayer(player_id, dir);
    this->players[player_id]->updatePose(std::move(new_pose));
}

void Gameloop::processBuyItem(Id player_id, Id npc_id, uint8_t type_item) {
    const auto trader = dynamic_cast<TraderNPC*>(this->npcs.at(npc_id));
    if (!trader) {
        return;  // Enviar error como "Este NPC no vende ni compra nada."
    }
    auto& tienda = trader->getStore();
    TypeItem tipo_buscado = static_cast<TypeItem>(type_item);
    if (!tienda.contains(tipo_buscado)) {
        return;  // Enviar error como "El NPC no vende ese ítem."
    }

    auto item_template = dynamic_cast<ShopItem*>(tienda.at(tipo_buscado).get());
    Player* player = this->players[player_id].get();

    if (!player->canBuy(item_template)) {
        return;  // Enviar error como "Oro insuficiente o inventario lleno."
    }

    Id id_instance = this->next_item_id++;
    player->buyItem(item_template, id_instance);
    player->breakMeditation();
}

void Gameloop::processSellItem(Id player_id, Id npc_id, Id id_item) {
    const auto trader = dynamic_cast<TraderNPC*>(this->npcs.at(npc_id));
    if (!trader) {
        return;  // Enviar error como "Este NPC no vende ni compra nada."
    }
    Player* player = this->players[player_id].get();
    if (!player->canSell(id_item)) {
        return;  // Error: "No tenés ese ítem en el inventario"
    }
    auto& tienda = trader->getStore();
    ItemInstance* itemInstance = player->getItemInstance(id_item);
    TypeItem tipo_buscado = itemInstance->type;
    if (!tienda.contains(tipo_buscado)) {
        return;  // Enviar error como "El NPC no compra ese ítem."
    }

    ShopItem* item_template = dynamic_cast<ShopItem*>(tienda.at(tipo_buscado).get());
    uint32_t sell_price = item_template->selling_price;
    player->sellItem(id_item, sell_price);
    player->breakMeditation();
}

void Gameloop::processPlayerPickUp(Id player_id) {
    auto player = this->players.at(player_id).get();
    Pose player_pose = player->getPose();

    ItemInstance* ground_item = this->world.getItemAt(player_pose.position);

    if (!ground_item) {  // Enviar error al cliente: "No hay nada tirado acá."
        return;
    }
    if (player->getInventorySize() >=
        player->getMaxInventorySize()) {  // Enviar error al cliente: "Inventario lleno."
        return;
    }

    std::unique_ptr<ItemInstance> picked_item = this->world.pickUpItem(player_pose.position);

    player->addItemToInventory(std::move(picked_item));

    player->breakMeditation();
}

void Gameloop::processPlayerDropItem(Id player_id, Id instance_id) {
    Player* player = this->players.at(player_id).get();
    if (!player->canSell(instance_id)) {
        // Enviar error al cliente: "No tenes ese item en el inventario."
        return;
    }
    Pose player_pose = player->getPose();
    if (!this->world.canDropItemAt(player_pose.position)) {
        // Enviar error: "No podes tirar objetos acá."
        return;
    }

    std::unique_ptr<ItemInstance> item_to_drop = player->removeItemFromInventory(instance_id);

    // this->world.dropItem(player_pose.position, std::move(item_to_drop));

    player->breakMeditation();
}

void Gameloop::processPlayerWithdrawItem(Id player_id, Id instance_id) {
    Player* player = this->players.at(player_id).get();
    if (!player->hasItemInBank(instance_id)) {
        // Error: "Ese objeto no está en tu cuenta bancaria."
        return;
    }
    if (player->getInventorySize() >= player->getMaxInventorySize()) {
        // Error: "No tenes espacio en el inventario para retirar esto."
        return;
    }
    std::unique_ptr<ItemInstance> item_to_withdraw = player->removeItemFromBank(instance_id);

    player->addItemToInventory(std::move(item_to_withdraw));
    player->breakMeditation();
}

void Gameloop::processPlayerDepositItem(Id player_id, Id instance_id) {
    Player* player = this->players.at(player_id).get();
    if (!player->canSell(instance_id)) {
        // Error: "No tenes ese item en el inventario."
        return;
    }
    if (player->getBankSize() >= player->getMaxBankSize()) {
        // Error: "No tenes espacio en el banco para depositar esto."
        return;
    }
    std::unique_ptr<ItemInstance> item_to_deposit = player->removeItemFromInventory(instance_id);

    player->addItemToBank(std::move(item_to_deposit));
    player->breakMeditation();
}

void Gameloop::processPlayerDepositGold(Id player_id, uint32_t amount) {
    Player* player = this->players.at(player_id).get();
    if (amount > player->getInventoryGold()) {
        // Error: "No tenes esa cantidad de oro en el inventario."
        return;
    }
    player->decreaseInventoryGold(amount);
    player->increaseBankGold(amount);
    player->breakMeditation();
}

void Gameloop::processPlayerWithdrawGold(Id player_id, uint32_t amount) {
    auto player = this->players.at(player_id).get();
    if (amount > player->getBankGold()) {
        // Error: "No tenes esa cantidad de oro en el banco."
        return;
    }
    player->decreaseBankGold(amount);
    player->increaseInventoryGold(amount);
    player->breakMeditation();
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

void Gameloop::processPlayerHeal(Id player_id) {
    Player* player = this->players.at(player_id).get();
    if (!player->isAlive()) {
        return;
    }
    player->restoreAllHp();
    player->restoreAllMana();
}

void Gameloop::processListItems(Id player_id, Id npc_id) {
    Player* player = this->players.at(player_id).get();
    if (!player)
        return;
    CitizenNPC* npc = this->npcs.at(npc_id);
    if (!npc)
        return;
    // TypeNPC npc_type = npc->getTypeNPC();
    //  NPC* npc_generico = this->npcs.at(npc_type).get();

    InteractionResult result = npc->interact();
    if (result.type == InteractionType::TRADER_SHOP) {
        // auto response = std::make_unique<ResponseTraderCatalog>(*(result.trader_store));
        // this->sendResponseToPlayer(player_id, std::move(response));
    } else if (result.type == InteractionType::BANK_BOX) {
        std::vector<MsgItemInfo> items_info = player->getBankItemsInfo();
        uint32_t bank_gold = player->getBankGold();
        auto response = std::make_unique<ResponseBankContent>(std::move(items_info), bank_gold);
        this->sendResponseToPlayer(player_id, std::move(response));
    }
}

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
    for (const auto& creature : this->creatures| std::views::values) {
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
            timer_attributes += TICK_MS;
            if (timer_attributes >= this->conf.times.update_player_atributes) {
                this->updatePlayersAttributes();
                timer_attributes = 0;
            }
            timer_spawn_npcs += TICK_MS;
            if (timer_spawn_npcs >= this->conf.times.spawn_npcs) {
                // this->respawnDeadNpcs();
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
    Thread::stop();             // Cambia el should_keep_running del Gameloop a false
    this->persistence.stop();   // <-- IMPORTANTE: Cambia el should_keep_running de la persistencia a false
}
Gameloop::~Gameloop() {
}
