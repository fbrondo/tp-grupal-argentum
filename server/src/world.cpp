#include "server/includes/world.h"

#include <iostream>

#include "common/includes/map/layer.h"
#include "server/print.h"

#include "printf.h"

World::World(const std::filesystem::path& path):map(MapSerializer::load(path)), limit_height(10/*this->map.height()*/), limit_width(10/*this->map.width()*/){
    this->buildTilesWorld();
}

void World::buildTilesWorld() {
    this->map_tiles.resize(this->limit_height, std::vector<Tile>(this->limit_width));
    for (uint32_t y = 0; y < this->limit_height; y++) {
        for (uint32_t x = 0; x < this->limit_width; x++) {
            const Tile& obj = this->map.tile_at(x, y, Layer::Object);
            const Tile& bg  = this->map.tile_at(x, y, Layer::Background);
            this->map_tiles[x][y].walkable = obj.walkable;
            this->map_tiles[x][y].region   = bg.region;
        }
    }
     /*Sabiendo cuentas regiones hay y donde se ubican, inicializo los npc*/
}

Position World::calculatePosition(const Id& player_id, const Direction dir) {
    const Position& pos = this->players_positions[player_id].position;
    Position new_pos;
    switch (dir) {
        case DOWN:
            new_pos.x = pos.x;
            new_pos.y = pos.y + 1;
            break;
        case UP:
            new_pos.x = pos.x;
            new_pos.y = pos.y - 1;
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
    for (auto& [id, player]: this->players_positions) {
        if (player.position == pos) {
            return true;
        }
    }

    /*verificamos que no hay un NPC*/
    for (auto& [id, instance_npc]: this->npcs_positions) {
        if (instance_npc.position == pos) {
            return true;
        }
    }
    /*verificamos si es colisionable*/
    if (!this->map_tiles[pos.x][pos.y].walkable) {
        return true;
    }
    return false;
}
/*Consultas para validar*/
bool World::isThisPlayerWithinTheLimits(const Id& player_id, const Direction dir) {
    const Position& pos = this->players_positions[player_id].position;
    switch (dir) {
        case DOWN:
            return (pos.y + 1 < this->limit_height);
        case UP:
            return (pos.y > 0);
        case LEFT:
            return (pos.x > 0);
        case RIGHT:
            return (pos.x + 1 < this->limit_width);
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

bool World::isSafeZONE(const Position& /*pos*/) {
    return true;
}

/*El estado del mundo cambia*/
void World::spawnPlayer(const Id& player_id) {
    /* Un nuevo jugador - recien registrado, su posicion sera en uno de los pueblos (zona segura)*/
    PlayerInstance player_inst(Position{4,4}, DOWN); /*La posicion esta harcodeada para probar*/
    this->players_positions.emplace(player_id, player_inst);
    Print::printPositionPlayerUpdate(player_id,this->players_positions.at(player_id));
}

void World::removePlayer(const Id& player_id) {
    this->players_positions.erase(player_id);
}

void World::movePlayer(const Id& player_id, Direction dir) {
    this->players_positions[player_id].position = this->calculatePosition(player_id, dir);;
    this->players_positions[player_id].direct = dir;
    Print::printPositionPlayerUpdate(player_id,this->players_positions.at(player_id));
}

const PlayerInstance& World::playerInformationInTheWorld(const Id& player_id) {
    return this->players_positions[player_id];
}
