#ifndef WORLD_H
#define WORLD_H

#include <filesystem>
#include <map>
#include <memory>
#include <random>
#include <unordered_map>
#include <vector>

#include "common/includes/core/position.h"
#include "common/includes/direction.h"
#include "common/includes/map/map.h"
#include "common/includes/map/tile.h"
#include "common/includes/types.h"
#include "server/includes/core/data.h"
#include "server/includes/core/instances.h"
#include "server/includes/core/item.h"
#include "server/includes/core/map.h"
#include "server/includes/items_positions.h"
#include "server/includes/npcs_positions.h"
#include "server/includes/player.h"

struct Zone {
    Id id;
    Region region;
    uint32_t tile_count{0};
    uint32_t creatures_count{0};
    uint32_t treasures_count{0};
    std::vector<Position> tiles;  // posiciones de todos los tiles de la zona
};

using MatrizBool = std::vector<std::vector<bool>>;
using MatrizMap = std::vector<std::vector<Tile>>;
using MapItems = std::map<TypeItem, std::unique_ptr<Item>>;
using Path = std::filesystem::path;

class World {
private:
    Id next_item_id{0};
    std::mt19937 gen;
    const Map map;
    const uint32_t limit_height;
    const uint32_t limit_width;
    MatrizMap map_tiles;
    // std::map<Region, uint32_t> zone_count;  // cuántas zonas hay de cada región
    // std::unordered_map<Id, Zone> zones;
    std::unordered_map<Id, Zone> safe_zones;
    std::unordered_map<Id, Zone> hostile_zones;
    std::unordered_map<Position, Id, PositionHash> position_zones;
    std::unordered_map<Position, bool, PositionHash> not_walkable_tiles;
    std::unordered_map<Position, bool, PositionHash> player_tiles;

    std::map<Id, Pose> players_positions;
    NpcPositions npc_positions;
    ItemsPositions item_positions;

    void buildTilesWorld();
    MatrizBool buildBackgroundVisualCoverage() const;
    void identifyZones();
    void floodFill(const Position pos_start, Region region, MatrizBool& visited, Zone& zone);

    bool positionNotWalkabled(const Position& pos) const;
    bool isInPlayerVisionRange(const Position& pos) const;
    bool isThisPlayerWithinTheLimits(const Id& player_id, const Direction dir);
    bool isWithinLimits(const Position& pos) const;
    Position calculatePosition(const Id& player_id, const Direction dir) const;

    Id calculateZoneSafeRandom();

public:
    World(const World& other) = delete;
    World& operator=(const World& other) = delete;

    explicit World(const Path& path);

    std::unordered_map<Id, Zone> getHostileZones();
    std::unordered_map<Id, Zone> getSafeZones();

    /*Consultas para validar*/
    const Map& getMap() const { return map; }

    bool isWalkable(const Id& id_player, const Direction dir);
    bool isCreatureWalkable(const Id& creature_id, Direction dir) const;
    bool isPositionInCreatureZone(const Id& creature_id, const Position& position) const;
    bool isSafeZONE(const Position& pos);

    Position calculatePositionRandom(const Id& zone_id);
    Position calculatePositionRandomSafeZone();
    Position findNearbyFreePosition(const Position& center) const;
    NpcInstance findNearestHealer(const Position& center) const;
    static uint32_t distanceBetweenPositions(const Position& from, const Position& to);

    void addPlayerWorld(const Id& player_id, const Pose& pose);
    void addNpcWorld(const NpcInstance& npc);
    void addItemWorld(const ItemInstance& item);
    void addItemWorld(const GoldBagInstance& gold);
    void addTreasuresWorld(const TreasureInstance& treasure);

    void removePlayer(const Id& player_id); /*Solo cuando un jugador se desconecte*/
    void removeCreature(const Id& creature_id);
    Pose movePlayer(const Id& player_id, Direction dir);
    Pose moveCreature(const Id& creature_id, Direction dir);
    Pose teleportPlayer(const Id& player_id, const Position& position);

    Position positionPlayerInTheWorld(const Id& player_id);
    bool playerTakeItemOnTheFloor(Player& player);

    int distanceBetweenTheAttackerAndTheVictim(const Id& attacker_id, const Id& victim_id);

    WorldStateData buildWorldState();
    std::vector<ItemGroundSnapshotData> itemsOnTheFloor();
};

#endif
