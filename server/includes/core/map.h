#ifndef MAP_H
#define MAP_H
#include <cstddef>
#include <functional>

#include "common/includes/core/position.h"
#include "common/includes/direction.h"

#pragma pack(push, 1)
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
