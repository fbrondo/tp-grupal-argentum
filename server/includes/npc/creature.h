#ifndef CRIATURE_H
#define CRIATURE_H

#include <vector>

#include "common/includes/types.h"
#include "server/includes/entity/combat_entity.h"
#include "server/includes/core/creature_attributes.h"
#include "server/includes/core/instances.h"
#include "server/includes/core/data.h"
#include "server/includes/core/map.h"
#include "server/includes/world.h"

/*Representa uba criatura
    - Goblin
    - Esqueleto
    - Zombies
    - Arana
    - Orco
    - Golem
    Una criatura puede atacar si un jugador esta en su rango o puede ser atacada por un jugador
*/
class Creature: public CombatEntity {
private:
    Id id;
    TypeNPC type_creature;
    uint16_t range_attack;
    uint32_t attack_cooldown_current{0};
    std::vector<ItemInstance> items_to_drop;
    ItemInstance search_item_drop(TypeItem type);

public:
    /*Tiempo de recuperacion al dar un ataque*/
    Creature(const Id& id_, TypeNPC type, const Pose& pose_, const NpcAttributes& attrib,
             const std::vector<ItemInstance>& items_);

    void onDeath(World& world) override;
    CreatureData getCreatureData();

};


#endif
