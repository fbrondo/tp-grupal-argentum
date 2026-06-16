#ifndef RACE_H
#define RACE_H

#include <string>
#include <utility>

#include "common/includes/core/Statistics.h"
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
    uint16_t hp_factor;       /*factor de vida*/
    uint16_t recovery_factor; /*factor de recuperacion de mana - Recup por tiempo*/
    uint16_t mana_factor;     /*factor de mana - Para el limite de ManaMax*/
    Statistics statistics;

    Race(TypeRace type, std::string&& name, uint16_t hp_f, uint16_t rec_f, uint16_t mana_f, const Statistics& st):
            type(type),
            name(std::move(name)),
            hp_factor(hp_f),
            recovery_factor(rec_f),
            mana_factor(mana_f),
            statistics(st) {}

    virtual ~Race() = default;
};


#endif
