#include "server/includes/world.h"

#include <stack>

#include "common/includes/map/layer.h"
#include "server/print.h"
World::World(const std::filesystem::path& path):
        map(MapSerializer::load(path)),
        limit_height(this->map.height()),
        limit_width(this->map.width()){
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

            this->zone_count[region]++;
            this->floodFill(Position{x, y}, region, visited, zone);
            this->zones.emplace(this->zone_count[region], std::move(zone));

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
        if (instance_npc.pose.position == pos) {
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
}

void World::removePlayer(const Id& player_id) { this->players_positions.erase(player_id); }

void World::movePlayer(const Id& player_id, Direction dir) {
    this->players_positions[player_id] = this->calculatePosition(player_id, dir);
    //Print::printPositionPlayerUpdate(player_id, this->players_positions.at(player_id));
}

Position World::positionPlayerInTheWorld(const Id& player_id) {
    return this->players_positions[player_id];
}

Position World::positionNPCInTheWorld(const Id& npc_id) {
    return this->npcs_positions[npc_id].pose.position;
}


int World::distanceBetweenTheAttackerAndTheVictim(const Id& attacker_id, const Id& victim_id) {
    const Position& pos_attacker =
            this->players_positions.at(attacker_id);                    // attacker->getPosition();
    const Position pos_target = this->players_positions.at(victim_id);  // victim->getPosition();
    int distance = std::abs(static_cast<int>(pos_attacker.x) - static_cast<int>(pos_target.x)) +
                   std::abs(static_cast<int>(pos_attacker.y) - static_cast<int>(pos_target.y));
    return distance;
}

// void World::spawnItemOnFloor(const Position& pos, TypeItem item_type) {
//     Id instance_id = this->next_item_instance_id++;
//
//     const Item& template_item = *(this->info_items.at(item_type));
//
//     ItemInstance new_item_instance(instance_id, item_type, template_item.getClassif(),
//                                    template_item.getBodyPart(), pos, 1);
//
//     this->items_on_flor.emplace(instance_id, std::move(new_item_instance));
//     return instance_id;
// }

// void World::spawnGoldOnFloor(const Position& pos, uint16_t amount) {
//     if (amount == 0) return;
//
//     for (auto& pile : this->gold_on_floor) {
//         if (pile.second.pos == pos) {
//             pile.second.amount += amount;
//             return;
//         }
//     }
//
//     GoldPile new_pile;
//     new_pile.amount = amount;
//     new_pile.pos = pos;
//
//     //Bueno aca faltaria asignar un id a la pila de oro
//     this->gold_on_floor.emplace(new_instance_id, std::move(new_pile));
// }
//
// void World::collectGoldAt(const Position& pos, Id& player_gold) {
//     auto it = this->gold_on_floor.begin();
//     while (it != this->gold_on_floor.end()) {
//         if (it->second.pos == pos) {
//             player_gold += it->second.amount;
//             it = this->gold_on_floor.erase(it);
//             return;
//         } else {
//             ++it;
//         }
//     }
// }