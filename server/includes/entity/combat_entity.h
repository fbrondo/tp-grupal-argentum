#pragma once
#include <string>

#include "common/includes/core/position.h"
#include "server/includes/core/data.h"
#include "server/includes/core/map.h"
#include "server/includes/entity/entity.h"
#include "server/includes/game_formulas.h"
class World;

class CombatEntity: public Entity {
protected:
    uint16_t hp;
    uint16_t max_hp;
    uint8_t level;
    uint32_t attack_cooldown_current{0};
    Pose pose;

public:
    ~CombatEntity() override = default;
    CombatEntity(const Pose& pose_, const PlayerData& player);
    CombatEntity(const Pose& pose_, uint16_t hp_max, uint8_t level);

    virtual bool isAlive() const;
    virtual bool isAttackable() override;
    virtual bool dodgeAttack() const;

    virtual void resetAttackCooldown(uint32_t cooldown_ms);
    virtual void updateAttackCooldown(uint32_t delta_ms);

    const Position& getPosition() const;
    uint8_t getLevel() const;
    uint16_t getMaxHp() const;

    virtual void receiveDamage(uint16_t damage, World& world);
    virtual void updatePose(Pose&& new_pose);
    virtual std::string getName() const = 0;
    virtual void onDeath(World& world) = 0;
};
