#include "server/includes/world.h"

#include <algorithm>
#include <optional>
#include <queue>
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

static std::optional<TypeNPC> citizenTypeFromDetailSprite(
        int sprite_id, const std::vector<CitizenDetailSpawnConfig>& spawn_configs) {
    for (const auto& spawn_config: spawn_configs) {
        if (spawn_config.detail_sprite_id == sprite_id) {
            return spawn_config.npc_type;
        }
    }
    return std::nullopt;
}

static uint32_t tilesCoveredBySpriteDimension(int pixels) {
    if (pixels <= 0) {
        return 1;
    }
    return static_cast<uint32_t>((pixels + TILE_SIZE - 1) / TILE_SIZE);
}

World::World(const std::filesystem::path& path,
             std::vector<CitizenDetailSpawnConfig> citizen_detail_spawns_):
        gen(std::random_device{}()),
        map(MapSerializer::load(path)),
        citizen_detail_spawns(std::move(citizen_detail_spawns_)),
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
            // const Tile& roof = this->map.tile_at(x, y, Layer::Roof);
            this->map_tiles[x][y].walkable = bg.walkable && details.walkable && obj.walkable &&
                                             /*roof.walkable &&*/ background_coverage[x][y] &&
                                             y >= TOP_ENTITY_VISUAL_MARGIN_TILES;
            this->map_tiles[x][y].region = bg.region;
            if (!this->map_tiles[x][y].walkable) {
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
            const int top_px = static_cast<int>(y) * TILE_SIZE;
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
        if (this->map_tiles[pos.x][pos.y].region != region)
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

void World::identifyZones() {
    MatrizBool visited(this->limit_height, std::vector<bool>(this->limit_width, false));
    Id zone_id = 0;
    for (uint32_t y = 0; y < this->limit_height; y++) {
        for (uint32_t x = 0; x < this->limit_width; x++) {
            if (visited[y][x])
                continue;
            /*Tile no visitado, es decir nueva zona*/
            const Region region = this->map_tiles[x][y].region;
            Zone zone;
            zone.region = region;
            zone.id = zone_id++;

            this->floodFill(Position{x, y}, region, visited, zone);
            const bool has_walkable_tile = std::any_of(
                    zone.tiles.begin(), zone.tiles.end(),
                    [this](const Position& pos) { return this->map_tiles[pos.x][pos.y].walkable; });
            if (!has_walkable_tile) {
                continue;
            }
            for (const Position& position: zone.tiles) {
                this->position_zones.emplace(position, zone.id);
            }
            if (region == Town || region == City) {
                this->safe_zones.emplace(zone.id, zone);
            } else {
                this->hostile_zones.emplace(zone.id, zone);
            }
        }
    }
}

std::vector<CitizenSpawnPoint> World::getCitizenSpawnPoints() const {
    std::vector<CitizenSpawnPoint> spawn_points;
    std::unordered_set<Position, PositionHash> used_positions;
    const std::filesystem::path sprites_path =
            std::filesystem::path(CONFIG_PATH).parent_path().parent_path() / "common" / "data" /
            "details_sprites.toml";
    const auto sprites = SpriteLoader::load(sprites_path);

    for (uint32_t y = 0; y < this->limit_height; ++y) {
        for (uint32_t x = 0; x < this->limit_width; ++x) {
            const Tile& detail = this->map.tile_at(x, y, Layer::Details);
            const auto type =
                    citizenTypeFromDetailSprite(detail.sprite_id, this->citizen_detail_spawns);
            if (!type.has_value()) {
                continue;
            }

            uint32_t width_tiles = 1;
            uint32_t height_tiles = 1;
            const auto sprite = sprites.find(detail.sprite_id);
            if (sprite != sprites.end()) {
                width_tiles = tilesCoveredBySpriteDimension(sprite->second.width);
                height_tiles = tilesCoveredBySpriteDimension(sprite->second.height);
            }

            Position center{x + width_tiles / 2, y + height_tiles / 2};
            if (!this->isWithinLimits(center)) {
                throw std::runtime_error("El centro del edificio de NPC queda fuera del mapa");
            }

            Position position = this->findNearbyFreePosition(center);
            if (!this->isWithinLimits(position) || !this->isSafeZONE(position)) {
                throw std::runtime_error(
                        "El punto de spawn de NPC debe quedar dentro de una ciudad o pueblo");
            }
            if (!used_positions.insert(position).second) {
                throw std::runtime_error(
                        "Dos NPC ciudadanos intentan spawnear en la misma posicion");
            }

            Id zone_id = 0;
            bool found_zone = false;
            for (const auto& [id, zone]: this->safe_zones) {
                if (std::find(zone.tiles.begin(), zone.tiles.end(), position) != zone.tiles.end()) {
                    zone_id = id;
                    found_zone = true;
                    break;
                }
            }
            if (!found_zone) {
                throw std::runtime_error("No se encontro la zona segura del punto de spawn de NPC");
            }

            spawn_points.push_back({zone_id, type.value(), Pose(position, DOWN)});
        }
    }

    return spawn_points;
}

Id World::calculateZoneSafeRandom() {
    if (this->safe_zones.empty()) {
        throw std::runtime_error("No hay zonas seguras configuradas en el mapa");
    }
    std::uniform_int_distribution<size_t> dist(0, this->safe_zones.size() - 1);
    size_t random_index = dist(this->gen);
    auto it = std::next(this->safe_zones.begin(), random_index);
    return it->first;
}

bool World::positionNotWalkabled(const Position& pos) const {
    return this->not_walkable_tiles.contains(pos);
}

bool World::isInPlayerVisionRange(const Position& pos) const {
    constexpr int32_t VISION_RANGE = 5;

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

bool World::isWalkable(const Id& player_id, const Direction dir) {
    if (!this->players_positions.contains(player_id)) {
        return false;
    }
    if (!this->isThisPlayerWithinTheLimits(player_id, dir)) {
        return false;
    }
    const Position position = this->calculatePosition(player_id, dir);
    if (this->positionNotWalkabled(position) || !this->map_tiles[position.x][position.y].walkable) {
        return false;
    }
    if (this->player_tiles.contains(position)) {
        return false;
    }
    if (this->npc_positions.isOcupied(position)) {
        return false;
    }
    return true;
}

bool World::isCreatureWalkable(const Id& creature_id, Direction dir) const {
    Position current = this->npc_positions.getPositionCreature(
            creature_id);  // this->npc_positions.getCreature(creature_id).pose.position;
    Position destination = current;
    switch (dir) {
        case DOWN:
            if (current.y + 1 >= this->limit_height)
                return false;
            destination.y++;
            break;
        case UP:
            if (current.y == 0)
                return false;
            destination.y--;
            break;
        case LEFT:
            if (current.x == 0)
                return false;
            destination.x--;
            break;
        case RIGHT:
            if (current.x + 1 >= this->limit_width)
                return false;
            destination.x++;
            break;
    }
    bool is_limits_zone = this->isPositionInCreatureZone(creature_id, destination);
    bool is_limits = this->isWithinLimits(destination);
    bool ocupied = !this->player_tiles.contains(destination) &&
                   !this->npc_positions.isOcupied(destination) &&
                   !this->item_positions.isOcupied(destination);
    bool walkable = this->map_tiles[destination.x][destination.y].walkable &&
                    !this->positionNotWalkabled(destination);
    return is_limits && is_limits_zone && ocupied && walkable;
}

bool World::isPositionInCreatureZone(const Id& creature_id, const Position& position) const {
    const Id creature_zone =
            this->npc_positions.getZoneCreature(creature_id);  // getCreature(creature_id).zone_id;
    const auto position_zone = this->position_zones.find(position);
    return position_zone != this->position_zones.end() && position_zone->second == creature_zone;
}

bool World::isSafeZONE(const Position& pos) const {
    for (const auto& [id, zone]: this->safe_zones) {
        for (const auto& position: zone.tiles) {
            if (position == pos) {
                return true;
            }
        }
    }
    return false;
}

Position World::calculatePosition(const Id& player_id, const Direction dir) const {
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
    const std::vector<Position>* tiles = nullptr;
    if (this->hostile_zones.contains(zone_id)) {
        tiles = &this->hostile_zones.at(zone_id).tiles;
    } else if (this->safe_zones.contains(zone_id)) {
        tiles = &this->safe_zones.at(zone_id).tiles;
    } else {
        throw std::runtime_error("La zona indicada no existe");
    }

    std::vector<Position> available;
    available.reserve(tiles->size());
    std::copy_if(tiles->begin(), tiles->end(), std::back_inserter(available),
                 [this](const Position& pos) {
                     const bool is_occupied = this->player_tiles.contains(pos) ||
                                              this->npc_positions.isOcupied(pos) ||
                                              this->item_positions.isOcupied(pos);
                     return this->map_tiles[pos.x][pos.y].walkable &&
                            !this->positionNotWalkabled(pos) && !this->isInPlayerVisionRange(pos) &&
                            !is_occupied;
                 });

    if (available.empty()) {
        throw std::runtime_error("No hay posiciones libres y caminables en la zona");
    }

    std::uniform_int_distribution<size_t> dist(0, available.size() - 1);
    return available[dist(this->gen)];
}
//
// Position World::findNearbyFreePosition(const Position& center) const {
//     const auto zone_it = this->zones.find(zone_id);
//     if (zone_it == this->zones.end()) {
//         return this->findAnyFreePosition();
//     }
//
//     std::vector<Position> free_tiles;
//     for (const Position& pos: zone->tiles) {
//         const bool occupied = this->player_tiles.contains(pos) ||
//                               this->npc_positions.isOcupied(pos) ||
//                               this->item_positions.isOcupied(pos);
//         if (this->isWithinLimits(pos) && this->map_tiles[pos.x][pos.y].walkable &&
//             !this->positionNotWalkabled(pos) && !this->isInPlayerVisionRange(pos) && !occupied) {
//             free_tiles.push_back(pos);
//         }
//     }
//
//     if (free_tiles.empty()) {
//         throw std::runtime_error("No hay posiciones libres en la zona");
//     }
//
//     std::uniform_int_distribution<size_t> dist(0, free_tiles.size() - 1);
//     return free_tiles[dist(this->gen)];
// }

Position World::calculatePositionRandomSafeZone() {
    Id zone_id = this->calculateZoneSafeRandom();
    return this->calculatePositionRandom(zone_id);
}

// bool World::zoneHasFreePosition(const Zone& zone) {
//     for (const Position& pos: zone.tiles) {
//         if (this->isWithinLimits(pos) && this->map_tiles[pos.x][pos.y].walkable &&
//             !this->isOccupied(pos)) {
//             return true;
//         }
//     }
//     return false;
// }

// Position World::findAnyFreePosition() {
//     for (uint32_t y = 0; y < this->limit_height; ++y) {
//         for (uint32_t x = 0; x < this->limit_width; ++x) {
//             Position pos(x, y);
//             if (this->map_tiles[x][y].walkable && !this->isOccupied(pos)) {
//                 return pos;
//             }
//         }
//     }
//
//     throw std::runtime_error("No hay posiciones libres en el mapa para spawnear");
// }

Position World::findNearbyFreePosition(const Position& center) const {
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

NpcInstance World::findNearestHealer(const Position& center) const {
    return this->npc_positions.findNearestPriest(center);
}

uint32_t World::distanceBetweenPositions(const Position& from, const Position& to) {
    const uint32_t dx = from.x > to.x ? from.x - to.x : to.x - from.x;
    const uint32_t dy = from.y > to.y ? from.y - to.y : to.y - from.y;
    return dx + dy;
}

std::unordered_map<Id, Zone> World::getHostileZones() { return this->hostile_zones; }
std::unordered_map<Id, Zone> World::getSafeZones() { return this->safe_zones; }


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

void World::removePlayer(const Id& player_id) {
    const Position& pos = this->players_positions[player_id].position;
    this->player_tiles.erase(pos);
    this->players_positions.erase(player_id);
}

void World::removeCreature(const Id& creature_id) {
    auto zone_id = this->npc_positions.removeCreature(creature_id);
    this->hostile_zones[zone_id].creatures_count--;
}

Pose World::movePlayer(const Id& player_id, Direction dir) {
    Position new_position = this->calculatePosition(player_id, dir);
    Position previous_position = this->players_positions[player_id].position;
    this->player_tiles.erase(previous_position);
    this->player_tiles.emplace(new_position, true);
    this->players_positions[player_id] = Pose(new_position, dir);
    Print::printPositionMovePlayer(player_id, this->players_positions[player_id],
                                   previous_position);
    return this->players_positions[player_id];
}

Pose World::moveCreature(const Id& creature_id, Direction dir) {
    const Position& current = this->npc_positions.getPositionCreature(creature_id);
    Position destination = current;
    switch (dir) {
        case DOWN:
            destination.y++;
            break;
        case UP:
            destination.y--;
            break;
        case LEFT:
            destination.x--;
            break;
        case RIGHT:
            destination.x++;
            break;
    }
    return this->npc_positions.moveCreature(creature_id, destination, dir);
}

Pose World::teleportPlayer(const Id& player_id, const Position& position) {
    Pose previous_pose = this->players_positions.at(player_id);
    this->player_tiles.erase(previous_pose.position);
    Position destination = this->findNearbyFreePosition(position);
    this->player_tiles.emplace(destination, true);
    Pose new_pose(destination, previous_pose.direct);
    this->players_positions[player_id] = new_pose;
    return new_pose;
}

// Position World::positionPlayerInTheWorld(const Id& player_id) {
//     return this->players_positions.at(player_id).position;
// }

int World::distanceBetweenTheAttackerAndTheVictim(const Id& attacker_id, const Id& victim_id) {
    const Position& pos_attacker = this->players_positions.at(attacker_id).position;
    const Position pos_target = this->players_positions.at(victim_id).position;
    int distance = std::abs(static_cast<int>(pos_attacker.x) - static_cast<int>(pos_target.x)) +
                   std::abs(static_cast<int>(pos_attacker.y) - static_cast<int>(pos_target.y));
    return distance;
}

bool World::playerTakeItemOnTheFloor(Player& player) {
    return this->item_positions.removeItemTakeToPlayer(player);
}

WorldStateData World::buildWorldState() {
    WorldStateData world_data;
    world_data.items = this->item_positions.getDataItems();
    world_data.gold_bags = this->item_positions.getDataGoldBags();
    world_data.treasures = this->item_positions.getDataTreasures();
    return world_data;
}

std::vector<ItemGroundSnapshotData> World::itemsOnTheFloor() {
    return this->item_positions.getItemsOnFloor();
}
