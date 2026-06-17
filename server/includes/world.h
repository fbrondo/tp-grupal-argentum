#ifndef WORLD_H
#define WORLD_H

#include <filesystem>
#include <map>
#include <memory>
#include <random>
#include <tuple>
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
#include "server/includes/npc/citizen_npc.h"
#include "server/includes/npc/trader.h"
#include "server/includes/player.h"
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
    std::vector<Position> tiles;  // posiciones de todos los tiles de la zona
};

using MatrizBool = std::vector<std::vector<bool>>;
using MatrizMap = std::vector<std::vector<Tile>>;
using MapItems = std::map<TypeItem, std::unique_ptr<Item>>;
using Path = std::filesystem::path;

class World {
private:
    Id& next_item_id;
    Map map;
    const uint32_t limit_height;
    const uint32_t limit_width;
    MatrizMap map_tiles;
    std::mt19937 gen;

    std::map<Region, uint32_t> zone_count;  // cuántas zonas hay de cada región
    std::map<Id, Zone> zones;               // todas las zonas identificadas
    // std::map<Region, uint32_t> region_count;
    std::vector<Id> safe_zones;  // spawn npc, new player o /resucitar

    std::unordered_map<Position, bool, PositionHash> occupied_tiles;

    std::map<Id, Pose> players_positions;
    std::map<Id, NpcInstance> creatures_positions;
    std::map<Id, NpcInstance> npc_positions;
    std::map<Id, Position> treausures_positions;
    std::map<Id, ItemInstance> items_on_flor;
    std::map<Id, GoldBagInstance> gold_on_floor;

    void buildTilesWorld();
    MatrizBool buildBackgroundVisualCoverage() const;
    void identifyZones();
    void floodFill(const Position pos_start, Region region, MatrizBool& visited, Zone& zone);
    void saveIdsOfTheSafeZones();


    bool isOccupied(const Position& pos) const;
    bool isThisPlayerWithinTheLimits(const Id& player_id, const Direction dir);
    bool isWithinLimits(const Position& pos) const;
    bool zoneHasFreePosition(const Zone& zone);
    Position findAnyFreePosition();

    Position calculatePosition(const Id& player_id, const Direction dir);

    Id calculateZoneSafeRandom();

public:
    World(const World& other) = delete;
    World& operator=(const World& other) = delete;

    explicit World(const Path& path, Id& next_item_id_);
    ~World() = default;

    const std::vector<std::tuple<Id, Region>> getZones();

    /*Consultas para validar*/
    const Map& getMap() const { return map; }

    bool isWalkable(const Id& id_player, const Direction dir);
    bool isFreePosition(const Position& pos) const;
    bool isSafeZONE(const Position& /*pos*/);
    bool canDropItemAt(const Position& pos);

    Position calculatePositionRandom(const Id& zone_id);
    Position calculatePositionRandomSafeZone();
    Position findNearbyFreePosition(const Position& center);

    void addPlayerWorld(const Id& player_id, const Pose& pose);
    void addCreatureWorld(const Id& creature_id, TypeNPC type, const Pose& pose);
    void addNpcWorld(const Id& npc_id, TypeNPC type, const Pose& pose);
    void addItemWorld(const ItemInstance& item);
    void addTreasuresWorld(const Id& treasures_id, const Position& position);

    void addGoldWorld(const Id& id, const GoldBagInstance& gold);
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

    NpcInstance* getNpcById(const Id& npc_id);
    ItemInstance* getItemAt(const Position& pos);
    std::unique_ptr<ItemInstance> pickUpItem(const Position& pos);
    // void dropItem(const Position& pos, std::unique_ptr<ItemInstance> item);

    int distanceBetweenTheAttackerAndTheVictim(const Id& attacker_id, const Id& victim_id);

    WorldStateData buildWorldState();
};

#endif
