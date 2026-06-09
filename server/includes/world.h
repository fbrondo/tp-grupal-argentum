#ifndef WORLD_H
#define WORLD_H

#include <cstdint>
#include <filesystem>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "common/includes/direction.h"
#include "common/includes/map/map.h"
#include "common/includes/map/map_serializer.h"
#include "common/includes/map/tile.h"
#include "common/includes/types.h"
#include "core/instances.h"
#include "core/item.h"
#include "core/map.h"
#include "npc/npc.h"

#include "player.h"


/*Representa mi mundo del juego:
    - Se sopne que recibo algo del editor para crear mi matriz
    - Una vez que se como es el mundo, esta clase sera la encargada modificar su estado
    - Tambien es la que me devuelve el estado del mundo.
*/

/*Responsabilidades*/
// Tener todo el conocimiento de los tiles del mundo.
// Saber que tipo de interaccion se tiene sobre una tiles
// Ubicar los NPCs y saber sus ubicaciones.
// Saber las pocisiones de los jugadores
// Cuando toque guardar
// Saber que zonas son seguras
// class Map;

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

class World {
private:
    Map map;
    const uint32_t limit_height;
    const uint32_t limit_width;

    std::vector<Zone> zones;                // todas las zonas identificadas
    std::map<Region, uint32_t> zone_count;  // cuántas zonas hay de cada región

    MatrizMap map_tiles;  // matriz
    std::map<Id, Position> players_positions;
    std::map<Id, NpcInstance> npcs_positions;
    std::map<Id, ItemInstace> items_on_flor;
    std::map<Region, uint32_t> region_count;

    void buildTilesWorld();
    void identifyZones();
    void floodFill(const Position pos_start, Region region, MatrizBool& visited, Zone& zone);
    Position calculatePosition(const Id& player_id, const Direction dir);

    bool isOccupied(const Position& pos);
    bool isThisPlayerWithinTheLimits(const Id& player_id, const Direction dir);

public:
    World(const World& other) = delete;
    World& operator=(const World& other) = delete;

    // World() = default;
    explicit World(const std::filesystem::path& path);

    ~World() = default;

    /*Consultas para validar*/
    const Map& getMap() const { return map; }

    bool isWalkable(const Id& id_player, const Direction dir);
    bool isSafeZONE(const Position& /*pos*/);

    /*void loadPlayer(const Id& player_id, Position&& position, Direction dir)*/
    void spawnPlayer(const Id& player_id);
    void removePlayer(const Id& player_id); /*Solo cuando un jugador se desconecte*/
    void movePlayer(const Id& player_id, Direction dir);
    Position positionPlayerInTheWorld(const Id& player_id);
};

#endif
