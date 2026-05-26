#ifndef WORLD_H
#define WORLD_H

#include <cstdint>
#include <filesystem>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

//#include "common/includes/map/map.h"
//#include "common/includes/map/map_serializer.h"
#include "common/includes/direction.h"
#include "common/includes/types.h"
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
// Saber que zonas son seguras
class Map;

struct Tile {
    TypeRegion type;
    bool colllisible;
    Id id_npc;
};

class World {
private:
    // Map map; /*viene del editor - solo se lee*/
    //  uint32_t limit_height;
    //  uint32_t limit_width;
    std::vector<std::vector<Tile>> map_tiles;  // matriz
    std::unordered_map<Id, Player&> players;
    std::unordered_map<Id, std::unique_ptr<NPC>> npcs;

    /*construye la matriz con lo que viene en map*/
    Position calculatePosition(const Id& player_id, const Direction dir);
    void buildTilesWorld();
    bool isOccupied(const Position& pos);
    bool isThisPlayerWithinTheLimits(const Id& player_id, const Direction dir);

public:
    World(const World& other) = delete;
    World& operator=(const World& other) = delete;

    explicit World(const std::filesystem::path& path);
    ~World() = default;

    /*Consultas para validar*/
    bool isWalkable(const Id& id_player, const Direction dir);
    bool isSafeZONE(const Position& pos);

    /*El estado del mundo cambia*/
    void addPlayer(const Id& player_id, Player& player);
    void removePlayer(const Id& player_id); /*Solo cuando un jugador se desconecte*/
    void movePlayer(const Id& player_id, Direction dir);
};

#endif
