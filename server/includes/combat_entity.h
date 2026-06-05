#pragma once

#include <utility>

#include "common/includes/types.h"
#include "server/includes/core/map.h"
#include "server/includes/entity.h"
#include "server/includes/game_formulas.h"

class CombatEntity: public Entity {
protected:
    uint16_t hp;
    uint16_t max_hp;

public:
    CombatEntity(Pose&& pos, uint16_t hp_max): Entity(std::move(pos)), hp(hp_max), max_hp(hp_max) {}
    bool isAlive() const { return this->hp > 0; }
    bool isAttackable() const override { return true; }

    virtual bool dodgeAttack() { return GameFormulas::calculationDodge(); }
    virtual void receiveDamage(uint16_t damage) {
        if (damage >= this->hp) {
            this->hp = 0;
            this->onDeath();
        } else {
            this->hp -= damage;
        }
    }
    virtual void onDeath() = 0;
};
