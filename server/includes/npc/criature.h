#ifndef CRIATURE_H
#define CRIATURE_H

#include <string>
#include <utility>

#include "../core/map.h"
#include "common/includes/types.h"

#include "npc.h"

/*Representa uba criatura
    - Goblin
    - Esqueleto
    - Zombies
    - Arana
    - Orco
    - Golem
    Una criatura puede atacar si un jugador esta en su rango o puede ser atacada por un jugador
*/
class Criature: public NPC {
private:
    uint16_t current_hp;

public:
    const uint16_t range_attack;
    const uint16_t hp_max_initial;
    const uint16_t level;
    /*Tiempo de recuperacion al dar un ataque*/

    Criature(TypeNPC type, std::string&& name, Position&& pos, uint16_t r_attack, uint16_t hp_max,
             uint16_t level);
    void updatePosition(Position&& new_pos);
    // void attack()
};


#endif
