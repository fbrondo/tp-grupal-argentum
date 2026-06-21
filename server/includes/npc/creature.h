#ifndef CRIATURE_H
#define CRIATURE_H
#include <string>
#include <vector>

#include "common/includes/core/snapshot.h"
#include "common/includes/types.h"
#include "server/includes/core/creature_attributes.h"
#include "server/includes/core/data.h"
#include "server/includes/core/instances.h"
#include "server/includes/core/map.h"
#include "server/includes/entity/combat_entity.h"
#include "server/includes/world.h"

class Creature: public CombatEntity {
private:
    Id id;
    std::string name;
    TypeNPC type_creature;
    uint16_t range_attack;
    uint32_t attack_cooldown_current{0};
    uint32_t movement_cooldown_current{0};
    std::vector<ItemInstance> items_to_drop;
    ItemInstance search_item_drop(TypeItem type);

public:
    /*Tiempo de recuperacion al dar un ataque*/
    Creature(const Id& id_, const std::string& name_, TypeNPC type, const Pose& pose_,
             const NpcAttributes& attrib, std::vector<ItemInstance>&& items_);

    void onDeath(World& world) override;
    CreatureData getCreatureData();
    NpcSnapshotData getNpcSnapshotData();
    std::string getName() const override;
    TypeNPC getTypeNPC() const { return type_creature; }
    uint16_t getAggroRange() const;
    bool canAttack() const;
    bool canMove() const;
    void resetAttackCooldown(uint32_t cooldown_ms);
    void resetMovementCooldown(uint32_t cooldown_ms);
    void updateCooldowns(uint32_t delta_ms);
    uint16_t calculateDamage(bool& is_critical) const;
};


#endif
