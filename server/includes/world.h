#ifndef WORLD_H
#define WORLD_H

#include <cstdint>
#include <filesystem>
#include <tuple>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <random>

#include "common/includes/direction.h"
#include "common/includes/map/map.h"
#include "common/includes/map/map_serializer.h"
#include "common/includes/map/tile.h"
#include "common/includes/types.h"
#include "server/includes/core/instances.h"
#include "server/includes/core/item.h"
#include "server/includes/core/map.h"
#include "server/includes/npc/citynpc.h"
#include "server/includes/npc/npc.h"
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


struct StateWorld {
    /*Lista de npc*/
    /*Lista de items en el piso*/
};

struct Zone {
    Id zone_id;
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
    Map map;
    const uint32_t limit_height;
    const uint32_t limit_width;
    MatrizMap map_tiles;
    std::mt19937 gen;
    Direction direction_spaw_default = UP; /*Tanto para jugadores como npcs*/

    std::map<Region, uint32_t> zone_count;  // cuántas zonas hay de cada región
    std::map<Id, Zone> zones;                // todas las zonas identificadas
    std::vector<Id> safe_zones; /*Para spaw de nuevo jugador haga spaw o ubicar jugador ./resucitar*/

    std::map<Id, Pose> players_positions;
    std::map<Id, NpcInstance> npcs_positions;
    std::map<Id, Position> treausures_positions;
    std::map<Id, ItemInstance> items_on_flor;
    std::map<Id, GoldPile> gold_on_floor;
    std::map<Region, uint32_t> region_count;


    void buildTilesWorld();
    void identifyZones();
    void floodFill(const Position pos_start, Region region, MatrizBool& visited, Zone& zone);
    void saveIdsOfTheSafeZones();


    bool isOccupied(const Position& pos);
    bool isThisPlayerWithinTheLimits(const Id& player_id, const Direction dir);

    Position calculatePosition(const Id& player_id, const Direction dir);
    Position calculatePositionRandom(const Id &zone_id);
    Id calculateZoneSafeRandom();

public:
    World(const World& other) = delete;
    World& operator=(const World& other) = delete;

    // World() = default;
    explicit World(const Path& path /*, const MapItems& info_items*/);

    ~World() = default;
    const std::vector<std::tuple<Id, Region>> getZones();
    /*Consultas para validar*/
    bool isWalkable(const Id& id_player, const Direction dir);
    bool isSafeZONE(const Position& /*pos*/);
    bool canDropItemAt(const Position& pos);

    void spawnTreasure(const Id& treasure_id, const Id& zone_id);

    Pose spawnNpc(TypeNPC type, const Id& npc_id, const Id& zone_id);
    Pose spawnPlayer(const Id& player_id);

    void removePlayer(const Id& player_id); /*Solo cuando un jugador se desconecte*/
    Pose movePlayer(const Id& player_id, Direction dir);

    // void spawnItemOnFloor(const Position& pos, TypeItem item_type);
    // void spawnGoldOnFloor(const Position& pos, uint16_t amount);
    // void collectGoldAt(const Position& pos, Id& player_gold);

    Position positionPlayerInTheWorld(const Id& player_id);
    Position positionNPCInTheWorld(const Id& npc_id);

    NpcInstance* getNpcById(const Id& npc_id);
    ItemInstance* getItemAt(const Position& pos);
    std::unique_ptr<ItemInstance> pickUpItem(const Position& pos);
    void dropItem(const Position& pos, std::unique_ptr<ItemInstance> item);

    int distanceBetweenTheAttackerAndTheVictim(const Id& attacker_id, const Id& victim_id);
};

#endif
