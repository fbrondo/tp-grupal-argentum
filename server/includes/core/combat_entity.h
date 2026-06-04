#pragma once

#include "entity.h"
#include "../game_formulas.h"
#include "common/includes/types.h"
#include "common/includes/direction.h"
#include "map.h"

class CombatEntity : public Entity {
protected:
    uint16_t hp;
    uint16_t max_hp;

public:
    CombatEntity(Id id, Position&& pos, uint16_t hp_max, GameFormulas& formulas)
        : Entity(id, std::move(pos), formulas), hp(hp_max), max_hp(hp_max) {}

    bool isAlive() const { return this->hp > 0; }
    bool isAttackable() const override { return true; }
    
    virtual bool dodgeAttack() { return this->form.calculationDodge(); }
    virtual void receiveDamage(uint16_t damage, const std::map<TypeItem, std::unique_ptr<Item>>& /*info_items*/) {
        if (damage >= this->hp) { 
            this->hp = 0; 
            this->onDeath(); 
        } else { 
            this->hp -= damage; 
        }
    }
    virtual void onDeath() = 0;
};