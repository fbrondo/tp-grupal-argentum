#ifndef MAP_H
#define MAP_H
#include <cstddef>
#include <functional>
#include <string>
#include <vector>

#include "common/includes/core/position.h"
#include "common/includes/direction.h"

#pragma pack(push, 1)
// struct HouseConfig {
//     Id id;
//     std::string npc_type;  // "Banquero", "Sacerdote", "Comerciante"
//     std::string npc_name;
// };
// struct HouseDetectionConfig {
//     std::vector<std::string> layers;  // "object", "details"
//     std::vector<int> house_sprite_ids;
//     std::vector<int> door_sprite_ids;
//     std::vector<HouseConfig> houses;
// };
// struct House {
//     Id id;
//     std::string name;
//     std::string npc_name;
//     std::vector<Position> tiles;  // tiles que ocupan los sprites de esta casa
//     Position entrance;            // posición de la puerta/entrada
// };

struct PositionHash {
    size_t operator()(const Position& pos) const {
        return std::hash<uint32_t>()(pos.x) ^ (std::hash<uint32_t>()(pos.y) << 32);
    }
};

struct Pose {
    Position position;
    Direction direct;

    Pose() = default;
    Pose(Position pos_, Direction direct_): position(pos_), direct(direct_) {}

    Pose(Pose&&) = default;
    Pose& operator=(Pose&&) = default;

    Pose(const Pose&) = default;
    Pose& operator=(const Pose&) = default;
};
#pragma pack(pop)
#endif
