#ifndef WORLD_H
#define WORLD_H

#include <filesystem>
#include <map>
#include <memory>
#include <random>
#include <unordered_map>
#include <vector>

#include "common/includes/direction.h"
#include "common/includes/map/map.h"
#include "common/includes/map/tile.h"
#include "common/includes/types.h"
#include "server/includes/core/data.h"
#include "server/includes/core/instances.h"
#include "server/includes/core/item.h"
#include "server/includes/core/map.h"
#include "server/includes/npcs_positions.h"
#include "server/includes/items_positions.h"
#include "server/includes/player.h"
#include "server/includes/player.h"
#include "server/includes/core/map.h"
/*Representa mi mundo del juego:
    - Se sopne que recibo algo del editor para crear mi matriz
    - Una vez que se como es el mundo, esta clase sera la encargada modificar su estado
    - Tambien es la que me devuelve el estado del mundo.
*/

/*Responsabilidades
    -Tener todo el conocimiento de los tiles del mundo.
    -Saber que tipo de interaccion se tiene sobre una tiles
    -Ubicar los NPCs y saber sus ubicaciones.
    -Saber las pocisiones de los jugadores
    - Cuando toque guardar
    - Saber que zonas son seguras
*/

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
    //std::map<Region, uint32_t> zone_count;  // cuántas zonas hay de cada región
    //std::unordered_map<Id, Zone> zones;
    std::unordered_map<Id, Zone> safe_zones;
    std::unordered_map<Id, Zone>  hostile_zones;
    std::unordered_map<Position, bool, PositionHash> not_walkable_tiles;
    std::unordered_map<Position, bool, PositionHash> player_tiles;

    std::map<Id, Pose> players_positions;
    NpcPositions npc_positions;
    ItemsPositions item_positions;

    // std::map<Id, NpcInstance> creatures_positions;
    // std::map<Id, NpcInstance> bankers_positions;
    // std::map<Id, NpcInstance> citizen_positions;
    // std::map<Id, NpcInstance> priest_positions;

    // std::map<Id, ItemInstance> items_on_flor;
    // std::map<Id, Position> treausures_positions;
    // std::map<Id, GoldBagInstance> gold_on_floor;

    void buildTilesWorld();
    void identifyZones();
    void floodFill(const Position pos_start, Region region, MatrizBool& visited, Zone& zone);

    bool positionNotWalkabled(const Position& pos) const;
    bool isInPlayerVisionRange(const Position& pos) const;
    bool isThisPlayerWithinTheLimits(const Id& player_id, const Direction dir);
    bool isWithinLimits(const Position& pos) const;

    Position calculatePosition(const Id& player_id, const Direction dir);

    Id calculateZoneSafeRandom();

public:
    World(const World& other) = delete;
    World& operator=(const World& other) = delete;

    explicit World(const Path& path);
    ~World() = default;

    std::unordered_map<Id, Zone> getHostileZones();
    std::unordered_map<Id, Zone> getSafeZones();

    //const std::map<Id, Pose> get_players_positions();
    // const std::map<Id, NpcInstance> get_creatures_positions();
    // const std::map<Id, NpcInstance> get_npc_positions();
    // const std::map<Id, Position> get_treausures_positions();
    // const std::map<Id, ItemInstance> get_items_on_flor();
    // const std::map<Id, GoldBagInstance> get_gold_on_floor();

    /*Consultas para validar*/
    const Map& getMap() const { return map; }

    bool isWalkable(const Id& id_player, const Direction dir);
    bool isSafeZONE(const Position& pos);
    //bool canDropItemAt(const Position& pos);

    Position calculatePositionRandom(const Id& zone_id);
    Position calculatePositionRandomSafeZone();
    Position findNearbyFreePosition(const Position& center) const;
    //Position findNearbyHealerPosition(const Position& center);

    void addPlayerWorld(const Id& player_id, const Pose& pose);
    void addNpcWorld(const NpcInstance& npc);
    void addItemWorld(const ItemInstance& item);
    void addItemWorld(const GoldBagInstance& gold);
    void addTreasuresWorld(TreasureInstance& treasure);

    /*Un critauta pierde la vida*/
    void spawnGoldOnFloor(const GoldBagInstance& gold);
    /*Inicializacion*/
    void spawnTreasure(const Id& treasure_id, const Id& zone_id);
    void spawnItemOnFloor(const ItemInstance& item);

    // void collectGoldAt(const Position& pos, Id& player_gold);
    void removePlayer(const Id& player_id); /*Solo cuando un jugador se desconecte*/
    void removeCreature(const Id& creature_id);
    Pose movePlayer(const Id& player_id, Direction dir);

    Position positionEntityTheWorld(const Id& id) const;
    Position positionPlayerInTheWorld(const Id& player_id);

    //NpcInstance* getNpcById(const Id& npc_id);
    void playerTakeItemOnTheFloor(Player& player);

    std::unique_ptr<ItemInstance> pickUpItem(const Position& pos);
    // void dropItem(const Position& pos, std::unique_ptr<ItemInstance> item);

    int distanceBetweenTheAttackerAndTheVictim(const Id& attacker_id, const Id& victim_id);

    WorldStateData buildWorldState();
};

#endif
