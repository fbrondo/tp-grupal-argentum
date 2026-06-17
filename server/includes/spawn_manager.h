#ifndef SPAWN_MANAGER_H
#define SPAWN_MANAGER_H
#include <map>
#include <memory>
#include <random>
#include <string>
#include <tuple>
#include <vector>

#include "common/includes/map/tile.h"
#include "server/includes/core/bank.h"
#include "server/includes/core/config.h"
#include "server/includes/core/region.h"
#include "server/includes/npc/citizen_npc.h"
#include "server/includes/npc/creature.h"
#include "server/includes/world.h"

class SpawnManager {
private:
    std::mt19937 gen;
    World& world;

    const std::map<TypeItem, std::unique_ptr<Item>>& items;
    const std::map<std::string, CreatureConfig>& conf_creatures;
    const std::map<std::string, NpcSafeZone>& conf_citizens;

    std::map<Region, HostileRegion*> hostile_regions;
    std::map<Region, SafeRegion*> safe_regions;

    std::map<TypeItem, Item*> items_store_citizen(const std::string& name_npc);
    std::vector<ItemInstance> items_drop_creature() const;
    NpcAttributes attributesToCreature(const CreatureConfig& creature);

    std::unique_ptr<Creature> createCreature(const Id& id, TypeNPC type, const Pose& pose,
                                             const NpcAttributes& attrib);
    std::unique_ptr<CitizenNPC> createCitizenNpc(const std::string& name_npc, Bank& bank);

    std::tuple<TypeNPC, NpcAttributes, Pose> prepareNpcSpawn(const Id& zone_id,
                                                             const HostileRegion& region,
                                                             const size_t& index_random);
    std::tuple<TypeNPC, Pose> prepareCitizenNpcSpawn(const Id& zone_id,
                                                     const std::string& name_npc);

    void prepareNewTreasure(const Id& zone_id, const HostileRegion& region);

public:
    SpawnManager(const GameConfig& conf, World& world_);
    void spawnTreasuresZones();
    void spawnCreaturesZones(Id& next_id, std::map<Id, std::unique_ptr<Creature>>& creatures);
    void spawnCitizenNpcZones(Id& next_id, std::map<Id, std::unique_ptr<CitizenNPC>>& citizen_npcs,
                              Bank& bank);
};


#endif  // SPAWN_MANAGER_H
