#include "../includes/world.h"

#define PRUEBA_HEIGHT 10
#define PRUEBA_WIDTH 10

World::World(const std::filesystem::path& path) {
    
}
void World::buildTilesWorld() {
    this->map_tiles.resize(PRUEBA_HEIGHT, std::vector<Tile>(PRUEBA_WIDTH));
    for (uint32_t x = 0; x < PRUEBA_WIDTH; x++) {
        for (uint32_t y = 0; y < PRUEBA_HEIGHT; y++) {
            this->map_tiles[x][y] = {FILED, false, 0};
        }
    }
    this->map_tiles[1][3] = {FILED, true, 0};
    this->map_tiles[3][3] = {FILED, true, 0};
}

Position World::calculatePosition(const Id& player_id, const Direction dir) {
    const Position& pos = this->players_positions[player_id];
    Position new_pos;
    switch (dir) {
        case DOWN:
            new_pos.x = pos.x;
            new_pos.y = pos.y - 1;
            break;
        case UP:
            new_pos.x = pos.x;
            new_pos.y = pos.y + 1;
            break;
        case LEFT:
            new_pos.x = pos.x - 1;
            new_pos.y = pos.y;
            break;
        case RIGHT:
            new_pos.x = pos.x + 1;
            new_pos.y = pos.y;
            break;
        default:
            break;
    }
    return new_pos;
}



bool World::isOccupied(const Position& pos) {
    /*Verificamos que no haya otro jugador*/
    for (auto& [id, position_player]: this->players_positions) {
        if (position_player == pos) {
            return true;
        }
    }

    /*verificamos que no hay un NPC*/
    for (auto& [id, instance_npc]: this->npcs_positions) {
        if (instance_npc.pos == pos) {
            return true;
        }
    }
    /*verificamos si es colisonable*/
    if (this->map_tiles[pos.x][pos.y].colllisible) {
        return true;
    }

    return false;
}
/*Consultas para validar*/
bool World::isThisPlayerWithinTheLimits(const Id& player_id, const Direction dir) {
    const Position& pos = this->players_positions[player_id];
    switch (dir) {
        case DOWN:
            return (pos.y + 1 < PRUEBA_HEIGHT);
        case UP:
            return (pos.y > 0);
        case LEFT:
            return (pos.x > 0);
        case RIGHT:
            return (pos.x + 1 < PRUEBA_WIDTH);
        default:
            break;
    }
    return false;
}
bool World::isWalkable(const Id& player_id, const Direction dir) {

    if (!this->isThisPlayerWithinTheLimits(player_id, dir)) {
        return false;
    }
    Position pos = this->calculatePosition(player_id, dir);
    if (this->isOccupied(pos)) {
        return false;
    }
    return true;
}

bool World::isSafeZONE(const Position& pos) {}

/*El estado del mundo cambia*/
void World::addPlayer(const Id& player_id) {
    Position player_position(4,4); /*Harcodeado para probar*/
    this->players_positions.insert({player_id, player_position});
}

void World::removePlayer(const Id& player_id) {

}

void World::movePlayer(const Id& player_id, Direction dir) {
    Position new_pos = this->calculatePosition(player_id, dir);
    this->players_positions[player_id] = new_pos;
}

const Position& World::getPositionPlayer(const Id& player_id) {
    return this->players_positions[player_id];
}
