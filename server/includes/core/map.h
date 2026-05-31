#ifndef MAP_H
#define MAP_H
//#include "common/includes/map/map.h"
//#include "common/includes/map/map_serializer.h"
#include "common/includes/types.h"

struct Position {
    uint32_t x;
    uint32_t y;

    /*Operador de igualdad*/
    bool operator==(const Position& other) const {
        return (x == other.x && y == other.y);
    }

    /*Operadot de desigualdad*/
    bool operator!=(const Position& other) const{
        return !(*this == other);
    }

    bool operator<(const Position& other) const {
        return x < other.x || (x == other.x && y < other.y);
    }

    Position() = default;
    Position(uint32_t x_, uint32_t y_): x(x_), y(y_) {}

    Position(Position&&) = default;
    Position& operator=(Position&&) = default;

    Position(const Position&) = default;
    Position& operator=(const Position&) = default;
};


#endif
