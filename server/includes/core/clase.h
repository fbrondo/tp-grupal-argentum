#ifndef CLASS_H
#define CLASS_H

#include <string>
#include <utility>

#include "common/includes/core/Statistics.h"
#include "common/includes/types.h"

/*Representa una clase
    - Mago
    - Paladin
    - Clerigo
    - Guerrero
*/
struct Clase {

    TypeClase type;
    std::string name;
    uint16_t hp_factor;         /*factor de vida*/
    uint16_t meditation_factor; /*factor de recuperacion de mana - Recup por tiempo*/
    uint16_t mana_factor;       /*factor de mana - Para el limite de ManaMax*/
    Statistics statistics;

    Clase(TypeClase type, std::string&& name, uint16_t hp_f, uint16_t med_f, uint16_t mana_f,
          const Statistics& st):
            type(type),
            name(std::move(name)),
            hp_factor(hp_f),
            meditation_factor(med_f),
            mana_factor(mana_f),
            statistics(st) {}

    ~Clase() = default;
};

#endif
