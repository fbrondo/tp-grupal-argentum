#include "server/includes/spawn_manager.h"

#include <ranges>

#include <toml++/impl/forward_declarations.hpp>

#include "server/includes/npc/banker.h"
#include "server/includes/npc/merchant.h"
#include "server/includes/npc/priest.h"
#include "server/print.h"

SpawnManager::SpawnManager(const GameConfig& conf, World& world_):
        gen(std::random_device{}()),
        world(world_),
        items(conf.items),
        conf_creatures(conf.creatures),
        conf_citizens(conf.npcs) {
    for (const auto& [type, region]: conf.regions) {
        if (!region->is_safe) {
            auto hostile = dynamic_cast<HostileRegion*>(region.get());
            this->hostile_regions.emplace(type, hostile);
        } else {
            auto safe = dynamic_cast<SafeRegion*>(region.get());
            this->safe_regions.emplace(type, safe);
        }
    }
}

std::map<TypeItem, Item*> SpawnManager::items_store_citizen(const std::string& name_npc) {
    const auto& items_store = this->conf_citizens.at(name_npc).ids_items_store;
    std::map<TypeItem, Item*> items_;
    for (const auto& type_item: items_store) {
        Item* item = this->items.at(type_item).get();
        items_.emplace(type_item, item);
    }
    return items_;
}

std::vector<ItemInstance> SpawnManager::items_drop_creature() const {
    std::vector<ItemInstance> items_to_drop;
    for (const auto& item: this->items | std::views::values) {
        ItemInstance item_inst;
        item_inst.item = item.get();
        items_to_drop.push_back(item_inst);
    }
    return items_to_drop;
}

NpcAttributes SpawnManager::attributesToCreature(const CreatureConfig& creature) {
    NpcAttributes attrib;
    attrib.max_hp = creature.hp_max_initial;
    attrib.current_hp = creature.hp_max_initial;
    attrib.range_attack = creature.attack_range;
    std::uniform_int_distribution<uint16_t> distr_lervel(creature.minimal_level,
                                                         creature.maximun_level);
    attrib.difficulty_level = distr_lervel(this->gen);
    return attrib;
}

std::tuple<std::string, TypeNPC, NpcAttributes, Pose> SpawnManager::prepareNpcSpawn(
        const Id& zone_id, const HostileRegion& region, const size_t& index_random) {
    const auto& name_npc = region.npc_types[index_random];
    const auto& type_npc = this->conf_creatures.at(name_npc).type;
    const auto position_spawn = this->world.calculatePositionRandom(zone_id);
    Pose pose_spawn(position_spawn, DOWN);
    NpcAttributes attrib = this->attributesToCreature(this->conf_creatures.at(name_npc));
    return {name_npc, type_npc, attrib, pose_spawn};
}

std::unique_ptr<Creature> SpawnManager::createCreature(const std::string& name_npc,
                                                       const NpcInstance& creature_instance,
                                                       const NpcAttributes& attrib) {
    std::vector<ItemInstance> items_ = this->items_drop_creature();
    return std::make_unique<Creature>(name_npc, creature_instance, attrib, std::move(items_));
}

void SpawnManager::spawnCreaturesZones(Id& next_id,
                                       std::map<Id, std::unique_ptr<Creature>>& creatures) {
    const std::unordered_map<Id, Zone> hostile_zones = this->world.getHostileZones();
    for (const auto& zone: hostile_zones | std::views::values) {
        const auto region = this->hostile_regions.at(zone.region);
        const uint16_t respawn_threshold = region->max_creatures / 2;
        uint16_t count = zone.creatures_count;
        if (count < respawn_threshold) {
            std::uniform_int_distribution<size_t> distr_npc(0, region->npc_types.size() - 1);
            while (count < region->max_creatures) {
                const Id id = next_id++;
                auto [name, type, attrib, pose] =
                        this->prepareNpcSpawn(zone.id, *region, distr_npc(this->gen));
                NpcInstance creature_instance(id, zone.id, type, pose);
                auto creature = this->createCreature(name, creature_instance, attrib);
                creatures.emplace(id, std::move(creature));
                this->world.addNpcWorld(creature_instance);
                count++;
            }
        }
    }
}

std::unique_ptr<CitizenNPC> SpawnManager::createCitizenNpc(const std::string& name_npc,
                                                           const NpcInstance& instance,
                                                           Bank& bank) {
    std::map<TypeItem, Item*> items_ = this->items_store_citizen(name_npc);
    switch (instance.type) {
        case PRIEST:
            return std::make_unique<Priest>(name_npc, instance, std::move(items_));
        case MERCHANT:
            return std::make_unique<Merchant>(name_npc, instance, std::move(items_));
        default:
            return std::make_unique<Banker>(name_npc, instance, bank);
    }
}
std::tuple<TypeNPC, Pose> SpawnManager::prepareCitizenNpcSpawn(const Id& zone_id,
                                                               const std::string& name_npc) {
    const auto type_npc = this->conf_citizens.at(name_npc).type;
    const auto position_spawn = this->world.calculatePositionRandom(zone_id);
    Pose pose_spawn(position_spawn, DOWN);
    return {type_npc, pose_spawn};
}

void SpawnManager::spawnCitizenNpcZones(Id& next_id,
                                        std::map<Id, std::unique_ptr<CitizenNPC>>& citizen_npcs,
                                        Bank& bank) {
    const std::unordered_map<Id, Zone> safe_zones = this->world.getSafeZones();
    for (const auto& zone: safe_zones | std::views::values) {
        auto region = this->safe_regions.at(zone.region);
        for (size_t j = 0; const auto& name_npc: region->npc_types) {
            const uint16_t numbers_npc = region->numbers_npc[j];
            for (size_t i = 0; i < numbers_npc; i++) {
                auto [type, pose] = this->prepareCitizenNpcSpawn(zone.id, name_npc);
                NpcInstance instance(next_id++, zone.id, type, pose);
                auto new_npc = this->createCitizenNpc(name_npc, instance, bank);
                citizen_npcs.emplace(instance.id, std::move(new_npc));
                this->world.addNpcWorld(instance);
            }
            j++;
        }
    }
}

void SpawnManager::prepareNewTreasure(const Id& zone_id, const HostileRegion& region) {
    std::uniform_int_distribution<size_t> dist(0, this->items.size() - 1);
    TreasureInstance instance;
    instance.zone_id = zone_id;
    for (auto i = 0; i < region.number_items_treasure; i++) {
        size_t random_index = dist(this->gen);
        auto it = std::next(this->items.begin(), random_index);
        ItemInstance item(it->second.get());
        instance.items.emplace_back(item);
    }
    if (region.min_treasure.has_value()) {
        instance.amount_golden = region.min_treasure.value();
    }
    instance.position = this->world.calculatePositionRandom(zone_id);
    this->world.addTreasuresWorld(instance);
}

void SpawnManager::spawnTreasuresZones() {
    const std::unordered_map<Id, Zone> hostile_zones = this->world.getHostileZones();
    for (const auto& zone: hostile_zones | std::views::values) {
        const auto region = this->hostile_regions.at(zone.region);
        uint16_t count = zone.treasures_count;
        if (region->min_treasure.has_value() && region->max_treasure.has_value() && count == 0) {
            std::uniform_int_distribution dist(region->min_treasure.value(),
                                               region->max_treasure.value());
            const uint16_t number_treasure = dist(this->gen);
            for (auto i = 0; i < number_treasure; i++) {
                this->prepareNewTreasure(zone.id, *region);
            }
        }
    }
}

std::unique_ptr<Creature> SpawnManager::loadCreature(const Id& creature_id,
                                                     const CreatureData& data) {
    std::string name(data.name);
    Pose pose(data.position, static_cast<Direction>(data.direction));
    NpcInstance instance(creature_id, data.zone_id, static_cast<TypeNPC>(data.type), pose);
    NpcAttributes attrib(data.attributes);
    this->world.addNpcWorld(instance);
    return this->createCreature(name, instance, attrib);
}

std::unique_ptr<CitizenNPC> SpawnManager::loadCitizen(const Id& npc_id, const CitizenNpcData& data,
                                                      Bank& bank) {
    std::string name(data.name);
    Pose pose(data.position, static_cast<Direction>(data.direction));
    NpcInstance instance(npc_id, data.zone_id, static_cast<TypeNPC>(data.type), pose);
    this->world.addNpcWorld(instance);
    return this->createCitizenNpc(name, instance, bank);
}

void SpawnManager::loadTreasuresZones(const std::vector<TreasureStateData>& data) {
    for (const auto& treasure: data) {
        TreasureInstance instance;
        instance.zone_id = treasure.zone_id;
        instance.position = treasure.position;
        for (const auto& type_item: treasure.types_items) {
            auto type = static_cast<TypeItem>(type_item);
            const auto& item = this->items.at(type);
            ItemInstance item_instance(item.get());
            instance.items.emplace_back(item_instance);
        }
        this->world.addTreasuresWorld(instance);
    }
}

void SpawnManager::loadGoldBags(const std::vector<GoldBagsData>& data) {
    for (const auto& gold: data) {
        GoldBagInstance instance;
        instance.position = gold.position;
        instance.amount = gold.amount;
        this->world.addItemWorld(instance);
    }
}

void SpawnManager::loadItems(const std::vector<ItemInstanceData>& data) {
    for (auto& [type, position]: data) {
        ItemInstance instance;
        auto type_item = static_cast<TypeItem>(type);
        instance.position = position;
        const auto& item = this->items.at(type_item);
        instance.item = item.get();
        this->world.addItemWorld(instance);
    }
}
