#ifndef MAP_H
#define MAP_H

#include "common/includes/direction.h"

struct Position {
    uint32_t x;
    uint32_t y;

    /*Operador de igualdad*/
    bool operator==(const Position& other) const { return (x == other.x && y == other.y); }

    /*Operadot de desigualdad*/
    bool operator!=(const Position& other) const { return !(*this == other); }

    bool operator<(const Position& other) const {
        return x < other.x || (x == other.x && y < other.y);
    }

    Position(): x(0), y(0) {}
    Position(uint32_t x_, uint32_t y_): x(x_), y(y_) {}

    Position(Position&&) = default;
    Position& operator=(Position&&) = default;

    Position(const Position&) = default;
    Position& operator=(const Position&) = default;
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

#endif
