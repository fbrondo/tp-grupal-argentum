#ifndef CRIATURE_H
#define CRIATURE_H

#include <string>
#include <utility>
#include <vector>

#include "../combat_entity.h"
#include "common/includes/types.h"
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
    uint16_t range_attack;
    uint16_t level;
    uint32_t attack_cooldown_current;
    TypeNPC type_creature;
    World& world;
    std::vector<TypeItem> drop_items_pool;

    // uint16_t drop_gold;

public:
    /*Tiempo de recuperacion al dar un ataque*/

    Creature(TypeNPC type, Pose&& pose_, uint16_t r_attack, uint16_t hp_max, uint16_t level,
             World& world);
    // void updatePosition(Pose&& new_pose) override;
    void onDeath() override;
};


#endif
