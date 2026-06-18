#ifndef POSITION_H
#define POSITION_H

#include <cstdint>
#pragma pack(push, 1)
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

#pragma pack(pop)
#endif
