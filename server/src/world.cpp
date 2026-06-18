#include "server/includes/world.h"

#include <ranges>
#include <stack>
#include <stdexcept>
#include <unordered_set>
#include <vector>

#include "common/includes/map/layer.h"
#include "common/includes/map/map_serializer.h"
#include "common/includes/sprite_loader.h"
#include "server/includes/core/map.h"
#include "server/print.h"

#ifndef CONFIG_PATH
#define CONFIG_PATH "server/config"
#endif

static constexpr int TILE_SIZE = 32;
static constexpr uint32_t TOP_ENTITY_VISUAL_MARGIN_TILES = 1;

World::World(const std::filesystem::path& path):
        gen(std::random_device{}()),
        map(MapSerializer::load(path)),
        limit_height(this->map.height()),
        limit_width(this->map.width()) {
    this->buildTilesWorld();
    this->identifyZones();
}

void World::buildTilesWorld() {
    const MatrizBool background_coverage = this->buildBackgroundVisualCoverage();
    this->map_tiles.resize(this->limit_width, std::vector<Tile>(this->limit_height));
    for (uint32_t y = 0; y < this->limit_height; y++) {
        for (uint32_t x = 0; x < this->limit_width; x++) {
            const Tile& bg = this->map.tile_at(x, y, Layer::Background);
            const Tile& details = this->map.tile_at(x, y, Layer::Details);
            const Tile& obj = this->map.tile_at(x, y, Layer::Object);
            const Tile& roof = this->map.tile_at(x, y, Layer::Roof);
            this->map_tiles[x][y].walkable = bg.walkable && details.walkable && obj.walkable &&
                                             roof.walkable && background_coverage[x][y] &&
                                             y >= TOP_ENTITY_VISUAL_MARGIN_TILES;
            this->map_tiles[x][y].region = bg.region;
            if (!obj.walkable) {
                this->not_walkable_tiles[Position(x, y)] = true;
            }
        }
    }
}

// Calcula las tiles cubiertas por sprites de background y propaga su walkable visual.
MatrizBool World::buildBackgroundVisualCoverage() const {
    MatrizBool has_background(this->limit_width, std::vector<bool>(this->limit_height, false));
    MatrizBool blocked_by_background(this->limit_width,
                                     std::vector<bool>(this->limit_height, false));
    const std::filesystem::path sprites_path =
            std::filesystem::path(CONFIG_PATH).parent_path().parent_path() / "common" / "data" /
            "background_sprites.toml";
    const auto sprites = SpriteLoader::load(sprites_path);

    for (uint32_t y = 0; y < this->limit_height; ++y) {
        for (uint32_t x = 0; x < this->limit_width; ++x) {
            const Tile& bg = this->map.tile_at(x, y, Layer::Background);
            if (bg.sprite_id == 0) {
                continue;
            }

            const auto sprite_it = sprites.find(bg.sprite_id);
            if (sprite_it == sprites.end()) {
                has_background[x][y] = true;
                if (!bg.walkable) {
                    blocked_by_background[x][y] = true;
                }
                continue;
            }

            const SpriteDefinition& sprite = sprite_it->second;
            const int left_px = static_cast<int>(x) * TILE_SIZE;
            const int top_px = static_cast<int>(y) * TILE_SIZE + TILE_SIZE - sprite.height;
            const int right_px = left_px + sprite.width;
            const int bottom_px = top_px + sprite.height;

            const int start_x = std::max(0, left_px / TILE_SIZE);
            const int start_y = std::max(0, top_px / TILE_SIZE);
            const int end_x =
                    std::min(static_cast<int>(this->limit_width) - 1, (right_px - 1) / TILE_SIZE);
            const int end_y =
                    std::min(static_cast<int>(this->limit_height) - 1, (bottom_px - 1) / TILE_SIZE);

            for (int covered_y = start_y; covered_y <= end_y; ++covered_y) {
                for (int covered_x = start_x; covered_x <= end_x; ++covered_x) {
                    has_background[covered_x][covered_y] = true;
                    if (!bg.walkable) {
                        blocked_by_background[covered_x][covered_y] = true;
                    }
                }
            }
        }
    }

    MatrizBool coverage(this->limit_width, std::vector<bool>(this->limit_height, false));
    for (uint32_t y = 0; y < this->limit_height; ++y) {
        for (uint32_t x = 0; x < this->limit_width; ++x) {
            coverage[x][y] = has_background[x][y] && !blocked_by_background[x][y];
        }
    }

    return coverage;
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
        if (visited[pos.y][pos.x])
            continue;
        if (this->map_tiles[pos.y][pos.x].region != region)
            continue;

        visited[pos.y][pos.x] = true;
        zone.tile_count++;
        zone.tiles.emplace_back(pos);

        if (pos.x + 1 < static_cast<uint32_t>(this->limit_width)) {
            stack.emplace(pos.x + 1, pos.y);  // puedo reutilizar calcular posicio
        }
        if (pos.x > 0) {
            stack.emplace(pos.x - 1, pos.y);  // puedo reutilizar calcular posicion
        }
        if (pos.y + 1 < static_cast<uint32_t>(this->limit_height)) {
            stack.emplace(pos.x, pos.y + 1);  // puedo reutilizar calcular posicion
        }
        if (pos.y > 0) {
            stack.emplace(pos.x, pos.y - 1);  // puedo reutilizar calcular posicion
        }
    }
}

void World::saveIdsOfTheSafeZones() {
    for (auto& [id, zone]: this->zones) {
        if ((zone.region == Town || zone.region == City) && this->zoneHasFreePosition(zone)) {
            this->safe_zones.push_back(id);
        }
    }
}

void World::identifyZones() {
    MatrizBool visited(this->limit_height, std::vector<bool>(this->limit_width, false));
    Id zone_id = 0;
    for (uint32_t y = 0; y < this->limit_height; y++) {
        for (uint32_t x = 0; x < this->limit_width; x++) {
            if (visited[y][x])
                continue;
            /*Tile no visitado, es decir nueva zona*/
            const Region region = this->map_tiles[y][x].region;
            Zone zone;
            zone.region = region;
            zone.id = zone_id++;
            // this->zone_count[region]++;
            this->floodFill(Position{x, y}, region, visited, zone);
            if (region == Town || region == City) {
                this->safe_zones.emplace(zone.id, zone);
            } else {
                this->hostile_zones.emplace(zone.id, zone);
            }
        }
    }
}

Id World::calculateZoneSafeRandom() {
    std::uniform_int_distribution<size_t> dist(0, this->safe_zones.size() - 1);
    size_t random_index = dist(this->gen);
    auto it = std::next(this->safe_zones.begin(), random_index);
    return it->first;
}

bool World::positionNotWalkabled(const Position& pos) const {
    return this->not_walkable_tiles.contains(pos);
}

bool World::isInPlayerVisionRange(const Position& pos) const {
    constexpr int32_t VISION_RANGE = 5;  // 10x10 → 5 tiles en cada dirección -> en archivo toml

    for (const auto& player_pos: this->players_positions | std::views::values) {
        const int x_player = static_cast<int32_t>(player_pos.position.x);
        const int y_player = static_cast<int32_t>(player_pos.position.y);
        const int x = static_cast<int32_t>(pos.x);
        const int y = static_cast<int32_t>(pos.y);

        const int32_t dx = std::abs(x - x_player);
        const int32_t dy = std::abs(y - y_player);

        if (dx <= VISION_RANGE && dy <= VISION_RANGE) {
            return true;
        }
    }
    return false;
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

bool World::isWalkable(const Id& player_id, const Direction dir) {
    if (!this->isThisPlayerWithinTheLimits(player_id, dir)) {
        return false;
    }
    const Position position = this->calculatePosition(player_id, dir);
    if (this->positionNotWalkabled(position)) {
        return false;
    }
    if (this->npc_positions.isOcupied(position)) {
        return false;
    }
    return true;
}

bool World::isSafeZONE(const Position& pos) {
    for (const auto& [id, zone]: this->safe_zones) {
        for (const auto& position: zone.tiles) {
            if (position == pos) {
                return true;
            }
        }
    }
    return false;
}

Position World::calculatePosition(const Id& player_id, const Direction dir) {
    const Position& pos = this->players_positions.at(player_id).position;
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
    std::vector<Position> tiles;
    if (this->hostile_zones.contains(zone_id)) {
        tiles = this->hostile_zones[zone_id].tiles;
    } else {
        tiles = this->safe_zones[zone_id].tiles;
    }
    std::uniform_int_distribution<size_t> dist(0, tiles.size() - 1);
    Position random;
    bool is_ocupied;
    do {
        random = tiles[dist(this->gen)];
        is_ocupied = this->player_tiles.contains(random) || this->npc_positions.isOcupied(random) ||
                     this->item_positions.isOcupied(random);
    } while (this->positionNotWalkabled(random) || this->isInPlayerVisionRange(random) ||
             is_ocupied);
    return random;
}

Position World::findNearbyFreePosition(const Position& center) const {
    const auto zone_it = this->zones.find(zone_id);
    if (zone_it == this->zones.end()) {
        return this->findAnyFreePosition();
    }

    std::vector<Position> free_tiles;
    for (const Position& pos: zone_it->second.tiles) {
        if (this->isWithinLimits(pos) && this->map_tiles[pos.x][pos.y].walkable &&
            !this->isOccupied(pos)) {
            free_tiles.push_back(pos);
        }
    }

    if (free_tiles.empty()) {
        return this->findAnyFreePosition();
    }

    std::uniform_int_distribution<size_t> distrib(0, free_tiles.size() - 1);
    return free_tiles[distrib(this->gen)];
}

Position World::calculatePositionRandomSafeZone() {
    Id zone_id = this->calculateZoneSafeRandom();
    return this->calculatePositionRandom(zone_id);
}

bool World::zoneHasFreePosition(const Zone& zone) {
    for (const Position& pos: zone.tiles) {
        if (this->isWithinLimits(pos) && this->map_tiles[pos.x][pos.y].walkable &&
            !this->isOccupied(pos)) {
            return true;
        }
    }
    return false;
}

Position World::findAnyFreePosition() {
    for (uint32_t y = 0; y < this->limit_height; ++y) {
        for (uint32_t x = 0; x < this->limit_width; ++x) {
            Position pos(x, y);
            if (this->map_tiles[x][y].walkable && !this->isOccupied(pos)) {
                return pos;
            }
        }
    }

    throw std::runtime_error("No hay posiciones libres en el mapa para spawnear");
}

Position World::findNearbyFreePosition(const Position& center) {
    std::queue<Position> queue;
    std::unordered_set<Position, PositionHash> visited;

    queue.push(center);
    visited.insert(center);

    while (!queue.empty()) {
        Position pos = queue.front();
        queue.pop();
        bool is_ocupied = this->player_tiles.contains(pos) || this->npc_positions.isOcupied(pos) ||
                          this->item_positions.isOcupied(pos);
        if (!is_ocupied && this->isWithinLimits(pos) && !this->positionNotWalkabled(pos) &&
            this->map_tiles[pos.x][pos.y].walkable) {
            return pos;
        }
        std::vector<Position> neighbors = {
                {pos.x + 1, pos.y},
                {pos.x - 1, pos.y},
                {pos.x, pos.y + 1},
                {pos.x, pos.y - 1},
        };

        for (auto& neighbor: neighbors) {
            if (!visited.contains(neighbor) && this->isWithinLimits(neighbor)) {
                visited.insert(neighbor);
                queue.push(neighbor);
            }
        }
    }
    return center;
}
//
// Position World::findNearbyHealerPosition(const Position& center) {
//     Position closest_position;
//     uint32_t min_distance = std::numeric_limits<uint32_t>::max();
//
//     // Mantenemos tu bucle con pipes intacto
//     for (const auto& npc: this->npc_positions | std::views::values) {
//         if (npc.type == PRIEST) {
//             uint32_t dist_x =
//                     std::abs(static_cast<int>(center.x) -
//                     static_cast<int>(npc.pose.position.x));
//             uint32_t dist_y =
//                     std::abs(static_cast<int>(center.y) -
//                     static_cast<int>(npc.pose.position.y));
//             uint32_t current_distance = dist_x + dist_y;
//
//             if (current_distance < min_distance) {
//                 min_distance = current_distance;
//                 closest_position = npc.pose.position;
//             }
//         }
//     }
//
//     return closest_position;
// }

// const std::map<Id, Pose> World::get_players_positions(){
//     return this->players_positions;
// }

bool World::isThisPlayerWithinTheLimits(const Id& player_id, const Direction dir) {
    const Position& pos = this->players_positions.at(player_id).position;
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

// const std::map<Id, NpcInstance> World::get_creatures_positions(){
//     return this->creatures_positions;
// }
//
// const std::map<Id, NpcInstance> World::get_npc_positions(){
//     return this->npc_positions;
// }
//
// const std::map<Id, ItemInstance> World::get_items_on_flor(){
//     return this->items_on_flor;
// }
//
// const std::map<Id, GoldBagInstance> World::get_gold_on_floor(){
//     return this->gold_on_floor;
// }


std::unordered_map<Id, Zone> World::getHostileZones() { return this->hostile_zones; }
bool World::isOccupied(const Position& pos) const {
    const auto it = this->occupied_tiles.find(pos);
    return it != this->occupied_tiles.end() && it->second;
}

bool World::isFreePosition(const Position& pos) const {
    return this->isWithinLimits(pos) && this->map_tiles[pos.x][pos.y].walkable &&
           !this->isOccupied(pos);
}

bool World::isWalkable(const Id& player_id, const Direction dir) {
    if (!this->players_positions.contains(player_id)) {
        return false;
    }
    if (!this->isThisPlayerWithinTheLimits(player_id, dir)) {
        return false;
    }
    const Position pos = this->calculatePosition(player_id, dir);
    if (!this->isFreePosition(pos)) {
        return false;
    }
    return true;
}

std::unordered_map<Id, Zone> World::getSafeZones() { return this->safe_zones; }


// bool World::canDropItemAt(const Position& pos) {
//     for (auto& [id, item]: this->items_on_flor) {
//         if (item.pos == pos) {
//             return false;
//         }
//     }
//     return true;
// }


void World::addPlayerWorld(const Id& player_id, const Pose& pose) {
    this->players_positions.emplace(player_id, pose);
    this->player_tiles.emplace(pose.position, true);
    /*auto it = this->players_positions.find(player_id);
    if (it != this->players_positions.end()) {
        this->occupied_tiles[it->second.position] = false;
        it->second = pose;
    } else {
        this->players_positions.emplace(player_id, pose);
    }
    this->occupied_tiles[pose.position] = true;*/
}

void World::addNpcWorld(const NpcInstance& npc) {
    Print::printNpc(npc);
    this->npc_positions.add(npc);
    if (this->hostile_zones.contains(npc.zone_id)) {
        this->hostile_zones[npc.zone_id].creatures_count++;
    }
}

void World::addItemWorld(const ItemInstance& item) {
    ItemInstance instance = item;
    instance.id = this->next_item_id++;
    this->item_positions.add(instance);
}

void World::addItemWorld(const GoldBagInstance& gold) {
    GoldBagInstance gold_instance(gold);
    gold_instance.id = this->next_item_id++;
    this->item_positions.add(gold_instance);
}

void World::addTreasuresWorld(const TreasureInstance& treasure) {
    TreasureInstance instance(treasure);
    instance.id = this->next_item_id++;
    this->item_positions.add(instance);
    this->hostile_zones[instance.zone_id].treasures_count++;
    Print::printItem(instance);
}

// void World::spawnItemOnFloor(const ItemInstance& item) {
//     ItemInstance new_item_instance(item);
//     Id instance_id = this->next_item_id++;
//     new_item_instance.id = instance_id;
//     this->items_on_flor.emplace(instance_id, new_item_instance);
//     this->occupied_tiles[new_item_instance.pos] = true;
// }

// void World::spawnGoldOnFloor(const GoldBagInstance& gold) {
//     Id id_gold = this->next_item_id++;
//     this->gold_on_floor.emplace(id_gold, gold);
//     this->occupied_tiles[gold.pos] = true;
// }

// void World::spawnTreasure(const Id& treasure_id, const Id& zone_id) {
//     Position random_position = this->calculatePositionRandom(zone_id);
//     this->treausures_positions.emplace(treasure_id, random_position);
//     this->occupied_tiles[random_position] = true;
// }


void World::removePlayer(const Id& player_id) {
    const Position& pos = this->players_positions[player_id].position;
    this->player_tiles.erase(pos);
    this->players_positions.erase(player_id);
}

void World::removeCreature(const Id& creature_id) {
    auto npc_instance = this->npc_positions.removeCreature(creature_id);
    this->hostile_zones[npc_instance.zone_id].creatures_count--;
}

Pose World::movePlayer(const Id& player_id, Direction dir) {
    Position new_position = this->calculatePosition(player_id, dir);
    Position previous_position = this->players_positions[player_id].position;
    this->player_tiles.erase(previous_position);
    this->player_tiles.emplace(new_position, true);
    Pose pose_move(new_position, dir);
    this->players_positions[player_id] = pose_move;
    /*Position previous_position = this->players_positions.at(player_id).position;
    Pose pose_move(new_position, dir);
    this->occupied_tiles[previous_position] = false;
    this->players_positions.at(player_id) = pose_move;
    this->occupied_tiles[new_position] = true;*/
    Print::printPositionMovePlayer(player_id, pose_move, previous_position);
    return pose_move;
}

Position World::positionPlayerInTheWorld(const Id& player_id) {
    return this->players_positions.at(player_id).position;
}

// Position World::positionEntityTheWorld(const Id& id) const {
//     if (this->npc_positions.contains(id)) {
//         return this->npc_positions.at(id).pose.position;
//     }
//     if (this->npc_positions.contains(id)) {
//         return this->creatures_positions.at(id).pose.position;
//     }
//     return this->players_positions.at(id).position;
// }
//
// NpcInstance* World::getNpcById(const Id& npc_id) { return &this->npc_positions[npc_id]; }


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
    // for (const auto& npc: this->npc_positions | std::views::values) {
    //     CitizenNpcData citizen_npc;
    //     citizen_npc.type = npc.type;
    //     citizen_npc.pos_x = npc.pose.position.x;
    //     citizen_npc.pos_y = npc.pose.position.y;
    //     citizen_npc.direction = npc.pose.direct;
    //     world_data.citizen_npcs.push_back(citizen_npc);
    // }
    //
    // for (const auto& treasure: this->treausures_positions | std::views::values) {
    //     TreasureStateData treas;
    //     treas.pos_x = treasure.x;
    //     treas.pos_y = treasure.y;
    //     world_data.treasures.push_back(treas);
    // }
    // for (const auto& gold_bags: this->gold_on_floor | std::views::values) {
    //     GoldBagsData gold;
    //     gold.pos_x = gold_bags.pos.x;
    //     gold.pos_y = gold_bags.pos.y;
    //     gold.amount = gold_bags.amount;
    //     world_data.gold_bags.push_back(gold);
    // }
    //
    // for (const auto& item_inst: this->items_on_flor | std::views::values) {
    //     ItemInstanceData item;
    //     item.type_item = item_inst.type;
    //     item.x = item_inst.pos.x;
    //     item.y = item_inst.pos.y;
    //     world_data.items.push_back(item);
    // }
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

void World::playerTakeItemOnTheFloor(Player& player) {
    this->item_positions.removeItemTakeToPlayer(player);
}

// std::unique_ptr<ItemInstance> World::pickUpItem(const Position& pos) {
//     auto it = this->items_on_flor.begin();
//     while (it != this->items_on_flor.end()) {
//         if (it->second.pos == pos) {
//             auto item_ptr = std::make_unique<ItemInstance>(std::move(it->second));
//             this->items_on_flor.erase(it);
//             return item_ptr;
//         } else {
//             ++it;
//         }
//     }
//     return nullptr;
// }

// void World::dropItem(const Position& pos, std::unique_ptr<ItemInstance> item) {
//     Id item_id = item->id;
//     this->items_on_flor.emplace(item_id, std::move(*item));
// }
