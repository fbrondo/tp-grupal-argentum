#ifndef RACE_H
#define RACE_H

#include <string>
#include <utility>

#include "common/includes/core/statics.h"
#include "common/includes/types.h"

/*Representa una raza
    - Humanos
    - Elfos
    - Enanos
    - Gnomos
*/
/*NOTA: Puede que el type sea inncesario para la raza - Puede servir creo que solo para el
 * protocolo*/
struct Race {
    TypeRace type;
    std::string name;
    Statics statics;

    uint16_t hpFactor;       /*factor de vida*/
    uint16_t recoveryFactor; /*factor de recuperacion de mana - Recup por tiempo*/
    uint16_t manaFactor;     /*factor de mana - Para el limite de ManaMax*/

    Race(TypeRace type, std::string&& name, Statics&& st, uint16_t lifeF, uint16_t manaRecF,
         uint16_t manaF):
            type(type),
            name(std::move(name)),
            statics(std::move(st)),
            hpFactor(lifeF),
            recoveryFactor(manaRecF),
            manaFactor(manaF) {}

    virtual ~Race() = default;
};


#endif
