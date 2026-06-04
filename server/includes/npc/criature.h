#ifndef CRIATURE_H
#define CRIATURE_H

#include <string>
#include <utility>
#include <vector>

#include "../core/combat_entity.h"
#include "../core/map.h"
#include "common/includes/types.h"

#include "npc.h"
#include "../world.h"

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
    World& world;
    TypeNPC type_creature;
    std::vector<TypeItem> drop_items_pool;
    uint16_t drop_gold;

public:
    const uint16_t range_attack;
    const uint16_t level;
    /*Tiempo de recuperacion al dar un ataque*/

    Creature(Id id, TypeNPC type, Position&& pos, uint16_t r_attack, uint16_t hp_max,
             uint16_t level, GameFormulas& formulas, World& world);
    void updatePosition(Position&& new_pos) override;
    void onDeath() override;
    // void attack()
};


#endif
