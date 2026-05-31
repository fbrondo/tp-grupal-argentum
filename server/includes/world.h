#ifndef WORLD_H
#define WORLD_H

#include <cstdint>
#include <filesystem>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include "common/includes/map/tile.h"
#include "common/includes/map/map.h"
#include "common/includes/map/map_serializer.h"
#include "common/includes/direction.h"
#include "common/includes/types.h"
#include "core/item.h"
#include "core/map.h"
#include "npc/npc.h"
#include "core/instances.h"
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
//class Map;

struct TileWorld {
    TypeRegion region;
    bool walkable;
};

struct StateWorld {
  
    /*Lista de npc*/
    /*Lista de items en el piso*/

};


class World {
private:
    Map map;
    const uint32_t limit_height;
    const uint32_t limit_width;
    std::vector<std::vector<TileWorld>> map_tiles;  // matriz
    std::map<Id, PlayerInstance> players_positions;
    std::map<Id, NpcInstance> npcs_positions;
    std::map<Id, ItemInstace> items_on_flor;

    void buildTilesWorld();
    /*construye la matriz con lo que viene en map*/
    Position calculatePosition(const Id& player_id, const Direction dir);
    
    bool isOccupied(const Position& pos);
    bool isThisPlayerWithinTheLimits(const Id& player_id, const Direction dir);

public:
    World(const World& other) = delete;
    World& operator=(const World& other) = delete;

    //World() = default;
    explicit World(const std::filesystem::path& path);
   
    ~World() = default;

    /*Consultas para validar*/
    bool isWalkable(const Id& id_player, const Direction dir);
    bool isSafeZONE(const Position& pos);

    /*void loadPlayer(const Id& player_id, Position&& position, Direction dir)*/
    void spawnPlayer(const Id& player_id);
    void removePlayer(const Id& player_id); /*Solo cuando un jugador se desconecte*/
    void movePlayer(const Id& player_id, Direction dir);
    const PlayerInstance& playerInformationInTheWorld(const Id& player_id);
};

#endif
