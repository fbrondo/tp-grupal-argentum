#include "server/includes/world.h"

#include <stack>

#include "common/includes/map/layer.h"
#include "server/print.h"
World::World(const std::filesystem::path& path):
        map(MapSerializer::load(path)),
        limit_height(10 /*this->map.height()*/),
        limit_width(10 /*this->map.width()*/) {
    this->buildTilesWorld();
    this->identifyZones();
    // Print::printinitMatrizMap(this->map_tiles, this->limit_height, this->limit_width);
}

void World::buildTilesWorld() {
    this->map_tiles.resize(this->limit_height, std::vector<Tile>(this->limit_width));
    for (uint32_t y = 0; y < this->limit_height; y++) {
        for (uint32_t x = 0; x < this->limit_width; x++) {
            const Tile& obj = this->map.tile_at(x, y, Layer::Object);
            const Tile& bg = this->map.tile_at(x, y, Layer::Background);
            this->map_tiles[x][y].walkable = obj.walkable;
            this->map_tiles[x][y].region = bg.region;
        }
    }
}

void World::floodFill(const Position pos_start, Region region, MatrizBool& visited, Zone& zone) {
    std::stack<Position> stack;
    stack.push(pos_start);
    while (!stack.empty()) {
        auto pos = stack.top();
        stack.pop();

        if (pos.x >= static_cast<uint32_t>(this->limit_width))
            continue;
        if (pos.y >= static_cast<uint32_t>(this->limit_height))
            continue;
        if (visited[pos.x][pos.y])
            continue;
        if (this->map_tiles[pos.x][pos.y].region != region)
            continue;

        visited[pos.x][pos.y] = true;
        zone.tile_count++;
        zone.tiles.push_back(Position{pos.x, pos.y});

        if (pos.x + 1 < static_cast<uint32_t>(this->limit_width)) {
            stack.push(Position{pos.x + 1, pos.y});  // puedo reutilizar calcular posicio
        }
        if (pos.x > 0) {
            stack.push({pos.x - 1, pos.y});  // puedo reutilizar calcular posicion
        }
        if (pos.y + 1 < static_cast<uint32_t>(this->limit_height)) {
            stack.push({pos.x, pos.y + 1});  // puedo reutilizar calcular posicion
        }
        if (pos.y > 0) {
            stack.push({pos.x, pos.y - 1});  // puedo reutilizar calcular posicion
        }
    }
}

void World::identifyZones() {
    MatrizBool visited(this->limit_width, std::vector<bool>(this->limit_height, false));
    for (uint32_t y = 0; y < this->limit_height; y++) {
        for (uint32_t x = 0; x < this->limit_width; x++) {
            if (visited[x][y])
                continue;
            /*Tile no visitado, es decir nueva zona*/
            Region region = this->map_tiles[x][y].region;
            Zone zone;
            zone.region = region;
            zone.zone_id = static_cast<uint32_t>(this->zones.size());

            this->floodFill(Position{x, y}, region, visited, zone);
            this->zones.push_back(std::move(zone));
            this->zone_count[region]++;
        }
    }
}

Position World::calculatePosition(const Id& player_id, const Direction dir) {
    const Position& pos = this->players_positions[player_id];
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
    for (auto& [id, position]: this->players_positions) {
        if (position == pos) {
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
    const Position& pos = this->players_positions[player_id];
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

bool World::isSafeZONE(const Position& /*pos*/) { return true; }

/*El estado del mundo cambia*/
void World::spawnPlayer(const Id& player_id) {

    Position position(Position{4, 4}); /*La posicion esta harcodeada para probar*/
    this->players_positions.emplace(player_id, position);
    // Print::printPositionPlayerUpdate(player_id, this->players_positions.at(player_id));
}

void World::removePlayer(const Id& player_id) { this->players_positions.erase(player_id); }

void World::movePlayer(const Id& player_id, Direction dir) {
    this->players_positions[player_id].position = this->calculatePosition(player_id, dir);
    this->players_positions[player_id].direct = dir;
    Print::printPositionPlayerUpdate(player_id, this->players_positions.at(player_id));
}

Position World::positionPlayerInTheWorld(const Id& player_id) {
    return this->players_positions[player_id];
}
