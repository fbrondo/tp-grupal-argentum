#include "server/includes/world.h"

#include <ranges>
#include <stack>
#include <unordered_set>

#include "common/includes/map/layer.h"
#include "common/includes/map/map_serializer.h"
#include "server/print.h"
World::World(const std::filesystem::path& path, Id& next_item_id_):
        next_item_id(next_item_id_),
        map(MapSerializer::load(path)),
        limit_height(this->map.height()),
        limit_width(this->map.width()),
        gen(std::random_device{}()) {
    this->buildTilesWorld();
    Print::printInitMatrizMap(this->map_tiles, this->limit_height, this->limit_width);
    this->identifyZones();
    this->saveIdsOfTheSafeZones();
}

void World::buildTilesWorld() {
    this->map_tiles.resize(this->limit_height, std::vector<Tile>(this->limit_width));
    for (uint32_t y = 0; y < this->limit_height; y++) {
        for (uint32_t x = 0; x < this->limit_width; x++) {
            const Tile& obj = this->map.tile_at(x, y, Layer::Object);
            const Tile& bg = this->map.tile_at(x, y, Layer::Background);
            this->map_tiles[x][y].walkable = obj.walkable;
            this->map_tiles[x][y].region = bg.region;
            if (!obj.walkable) {
                this->occupied_tiles[Position(x, y)] = true;
            } else {
                this->occupied_tiles[Position(x, y)] = false;
            }
        }
    }
    Print::imprimirTilesOcupadas(this->occupied_tiles);
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

void World::saveIdsOfTheSafeZones() {
    for (auto& [id, zone]: this->zones) {
        if (zone.region == Town || zone.region == City) {
            this->safe_zones.push_back(id);
        }
    }
}

void World::identifyZones() {
    MatrizBool visited(this->limit_width, std::vector<bool>(this->limit_height, false));
    Id zone_id = 0;
    for (uint32_t y = 0; y < this->limit_height; y++) {
        for (uint32_t x = 0; x < this->limit_width; x++) {
            if (visited[x][y])
                continue;
            /*Tile no visitado, es decir nueva zona*/
            Region region = this->map_tiles[x][y].region;
            Zone zone;
            zone.region = region;
            zone_id++;
            zone.id = zone_id;


            //this->zone_count[region]++;
            this->floodFill(Position{x, y}, region, visited, zone);
            this->zones.emplace(zone_id, std::move(zone));
        }
    }
}

Id World::calculateZoneSafeRandom() {
    std::uniform_int_distribution<size_t> dist(0, this->safe_zones.size() - 1);
    return this->safe_zones[dist(this->gen)];
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

Position World::calculatePositionRandom(const Id& zone_id) {
    const std::vector<Position>& tiles = this->zones[zone_id].tiles;
    std::uniform_int_distribution<size_t> distrib(0, tiles.size() - 1);
    Position random_postion = tiles[distrib(this->gen)];
    while (this->isOccupied(random_postion)) {
        random_postion = tiles[distrib(this->gen)];
    }
    return random_postion;
}

Position World::calculatePositionRandomSafeZone() {
    std::uniform_int_distribution<size_t> distrib_zone(0, this->safe_zones.size() - 1);
    Id zone_id = this->safe_zones[distrib_zone(this->gen)];
    return this->calculatePositionRandom(zone_id);
}

Position World::findNearbyFreePosition(const Position& center) {
    std::queue<Position> queue;
    std::unordered_set<Position, PositionHash> visited;

    queue.push(center);
    visited.insert(center);

    while (!queue.empty()) {
        Position pos = queue.front();
        queue.pop();

        if (!this->isOccupied(pos) && this->isWithinLimits(pos)) {
            return pos;
        }
        std::vector<Position> neighbors = {
                {pos.x + 1, pos.y},
                {pos.x - 1, pos.y},
                {pos.x, pos.y + 1},
                {pos.x, pos.y - 1},
        };

        for (auto& neighbor: neighbors) {
            if (!visited.count(neighbor) && this->isWithinLimits(neighbor)) {
                visited.insert(neighbor);
                queue.push(neighbor);
            }
        }
    }
    return center;
}

bool World::isWithinLimits(const Position& pos) const {
    return (pos.x < this->limit_width) && (pos.y < this->limit_height);
}

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

const std::vector<std::tuple<Id, Region>> World::getZones() {
    std::vector<std::tuple<Id, Region>> info_zones;
    for (auto [id, zone]: this->zones) {
        info_zones.push_back(std::make_tuple(id, zone.region));
    }
    return info_zones;
}

bool World::isOccupied(const Position& pos) { return this->occupied_tiles[pos]; }

bool World::isWalkable(const Id& player_id, const Direction dir) {
    if (!this->isThisPlayerWithinTheLimits(player_id, dir)) {
        return false;
    }
    const Position pos = this->calculatePosition(player_id, dir);
    if (this->isOccupied(pos)) {
        return false;
    }
    return true;
}

bool World::canDropItemAt(const Position& pos) {
    for (auto& [id, item]: this->items_on_flor) {
        if (item.pos == pos) {
            return false;
        }
    }
    return true;
}


void World::addPlayerWorld(const Id& player_id, const Pose& pose) {
    this->players_positions.emplace(player_id, pose);
    this->occupied_tiles[pose.position] = true;
}

void World::addCreatureWorld(const Id& creature_id, TypeNPC type, const Pose& pose) {
    NpcInstance new_npc(type, pose);
    this->creatures_positions.emplace(creature_id, new_npc);
    this->occupied_tiles[pose.position] = true;
}

void World::addNpcWorld(const Id& npc_id, TypeNPC type, const Pose& pose) {
    NpcInstance new_npc(type, pose);
    this->creatures_positions.emplace(npc_id, new_npc);
    this->occupied_tiles[pose.position] = true;
}

void World::addItemWorld(const ItemInstance& item) {
    this->items_on_flor.emplace(item.id, item);
    this->occupied_tiles[item.pos] = true;
}

void World::addTreasuresWorld(const Id &treasures_id, const Position &position) {
    this->treausures_positions.emplace(treasures_id, position);
    this->occupied_tiles[position] = true;
}

void World::addGoldWorld(const Id& id, const GoldBagInstance& gold) {
    this->gold_on_floor.emplace(id, gold);
    this->occupied_tiles[gold.pos] = true;
}

void World::spawnItemOnFloor(const ItemInstance& item) {
    ItemInstance new_item_instance(item);
    Id instance_id = this->next_item_id++;
    new_item_instance.id = instance_id;
    this->items_on_flor.emplace(instance_id, new_item_instance);
    this->occupied_tiles[new_item_instance.pos] = true;
}
void World::spawnGoldOnFloor(const GoldBagInstance& gold) {
    Id id_gold = this->next_item_id++;
    this->gold_on_floor.emplace(id_gold, gold);
    this->occupied_tiles[gold.pos] = true;
}

// void World::spawnTreasure(const Id& treasure_id, const Id& zone_id) {
//     Position random_position = this->calculatePositionRandom(zone_id);
//     this->treausures_positions.emplace(treasure_id, random_position);
//     this->occupied_tiles[random_position] = true;
// }

bool World::isSafeZONE(const Position& /*pos*/) { return true; }


void World::removePlayer(const Id& player_id) {
    const Position& pos = this->players_positions[player_id].position;
    this->occupied_tiles[pos] = false;
    this->players_positions.erase(player_id);
}

void World::removeCreature(const Id& creature_id) {
    const Position& pos = this->creatures_positions[creature_id].pose.position;
    this->occupied_tiles[pos] = false;
    this->creatures_positions.erase(creature_id);
}

Pose World::movePlayer(const Id& player_id, Direction dir) {
    Position new_position = this->calculatePosition(player_id, dir);
    Position previous_position = this->players_positions[player_id].position;
    Pose pose_move(new_position, dir);
    this->occupied_tiles[previous_position] = false;
    this->players_positions[player_id] = pose_move;
    Print::printPositionMovePlayer(player_id, pose_move, previous_position);
    return pose_move;
}

Position World::positionPlayerInTheWorld(const Id& player_id) {
    return this->players_positions[player_id].position;
}

Position World::positionEntityTheWorld(const Id& id) const {
    if (this->npc_positions.contains(id)) {
        return this->npc_positions.at(id).pose.position;
    }
    if (this->npc_positions.contains(id)) {
        return this->creatures_positions.at(id).pose.position;
    }
    return this->players_positions.at(id).position;
}

NpcInstance* World::getNpcById(const Id& npc_id) { return &this->npc_positions[npc_id]; }


int World::distanceBetweenTheAttackerAndTheVictim(const Id& attacker_id, const Id& victim_id) {
    const Position& pos_attacker =
            this->players_positions.at(attacker_id).position;  // attacker->getPosition();
    const Position pos_target =
            this->players_positions.at(victim_id).position;  // victim->getPosition();
    int distance = std::abs(static_cast<int>(pos_attacker.x) - static_cast<int>(pos_target.x)) +
                   std::abs(static_cast<int>(pos_attacker.y) - static_cast<int>(pos_target.y));
    return distance;
}

WorldStateData World::buildWorldState() {
    WorldStateData world_data;
    for (const auto& npc: this->npc_positions | std::views::values) {
        CitizenNpcData citizen_npc;
        citizen_npc.type = npc.type;
        citizen_npc.pos_x = npc.pose.position.x;
        citizen_npc.pos_y = npc.pose.position.y;
        citizen_npc.direction = npc.pose.direct;
        world_data.citizen_npcs.push_back(citizen_npc);
    }

    for (const auto& treasure: this->treausures_positions | std::views::values) {
        TreasureStateData treas;
        treas.pos_x = treasure.x;
        treas.pos_y = treasure.y;
        world_data.treasures.push_back(treas);
    }
    for (const auto& gold_bags: this->gold_on_floor | std::views::values) {
        GoldBagsData gold;
        gold.pos_x = gold_bags.pos.x;
        gold.pos_y = gold_bags.pos.y;
        gold.amount = gold_bags.amount;
        world_data.gold_bags.push_back(gold);
    }

    for (const auto& item_inst: this->items_on_flor | std::views::values) {
        ItemInstanceData item;
        item.type_item = item_inst.type;
        item.x = item_inst.pos.x;
        item.y = item_inst.pos.y;
        world_data.items.push_back(item);
    }
    return world_data;
}


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

ItemInstance* World::getItemAt(const Position& pos) {
    for (auto& [id, item]: this->items_on_flor) {
        if (item.pos == pos) {
            return &item;
        }
    }
    return nullptr;
}

std::unique_ptr<ItemInstance> World::pickUpItem(const Position& pos) {
    auto it = this->items_on_flor.begin();
    while (it != this->items_on_flor.end()) {
        if (it->second.pos == pos) {
            auto item_ptr = std::make_unique<ItemInstance>(std::move(it->second));
            this->items_on_flor.erase(it);
            return item_ptr;
        } else {
            ++it;
        }
    }
    return nullptr;
}

// void World::dropItem(const Position& pos, std::unique_ptr<ItemInstance> item) {
//     Id item_id = item->id;
//     this->items_on_flor.emplace(item_id, std::move(*item));
// }
