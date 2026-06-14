#pragma once
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
    Pose pose;

public:
    ~CombatEntity() override = default;
    CombatEntity(const Pose& pose_, const PlayerData& player);
    // CombatEntity(const Pose &pose, const NpcStateData& npc);
    CombatEntity(const Pose& pose_, uint16_t hp_max, uint8_t level);

    bool isAlive() const;
    bool isAttackable() override;

    const Position& getPosition() const;
    uint8_t getLevel() const;

    virtual bool dodgeAttack() const;
    virtual void receiveDamage(uint16_t damage, World& world);
    virtual void updatePose(Pose&& new_pose);
    virtual void onDeath(World& world) = 0;
};
