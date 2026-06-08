#pragma once

class Entity {

public:
    Entity() = default;
    virtual ~Entity() = default;
    virtual bool isAttackable() = 0;
};
