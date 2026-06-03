#pragma once 

#include <cstdint>

#include "../game_formulas.h"
#include "common/includes/types.h"
#include "common/includes/direction.h"

class Entity {
protected:
    Id id;
    Position pos;
    Direction direct;
    GameFormulas& form;

public:
    Entity(Id id, Position&& pos, GameFormulas& formulas) 
        : id(id), pos(std::move(pos)), direct(Direction::DOWN), form(formulas) {}

    virtual ~Entity() = default;

    Position getPosition() const { return this->pos; }
    virtual void updatePosition(Position&& new_pos) { this->pos = std::move(new_pos); }

    virtual bool isAttackable() const { return false; }
};