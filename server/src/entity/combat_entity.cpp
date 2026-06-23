#include "server/includes/entity/combat_entity.h"

#include "server/includes/core/map.h"
#include "server/includes/game_formulas.h"

CombatEntity::CombatEntity(const Pose& pose_, const PlayerData& player):
        hp(player.hp), level(player.level), pose(pose_) {}

CombatEntity::CombatEntity(const Pose& pos, uint16_t hp_max, uint8_t level):
        hp(hp_max), max_hp(hp_max), level(level), pose(pos) {}

bool CombatEntity::isAlive() const { return this->hp > 0; }

bool CombatEntity::isAttackable() { return true; }

void CombatEntity::resetAttackCooldown(uint32_t cooldown_ms) {
    this->attack_cooldown_current = cooldown_ms;
}

void CombatEntity::updateAttackCooldown(uint32_t delta_ms) {
    this->attack_cooldown_current = delta_ms >= this->attack_cooldown_current ?
                                            0 :
                                            this->attack_cooldown_current - delta_ms;
}

bool CombatEntity::dodgeAttack() const { return GameFormulas::calculationDodge(); }

const Position& CombatEntity::getPosition() const { return this->pose.position; }

uint8_t CombatEntity::getLevel() const { return this->level; }

uint16_t CombatEntity::getMaxHp() const { return max_hp; }

void CombatEntity::updatePose(Pose&& new_pose) { this->pose = std::move(new_pose); }

void CombatEntity::receiveDamage(uint16_t damage, World& world) {
    if (!this->isAlive()) {
        return;
    }
    if (damage >= this->hp) {
        this->hp = 0;
        this->onDeath(world);
    } else {
        this->hp -= damage;
    }
}
