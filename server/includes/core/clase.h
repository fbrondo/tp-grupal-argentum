#ifndef CLASS_H
#define CLASS_H

#include <string>
#include <utility>

#include "common/includes/core/statics.h"
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
    Statics statics;

    uint16_t hpFactor;         /*factor de vida*/
    uint16_t meditationFactor; /*factor de recuperacion de mana - Recup por tiempo*/
    uint16_t manaFactor;       /*factor de mana - Para el limite de ManaMax*/

    Clase(TypeClase type, std::string&& name, Statics&& st, uint16_t lifeF, uint16_t medF,
          uint16_t manaF):
            type(type),
            name(std::move(name)),
            statics(std::move(statics)),
            hpFactor(lifeF),
            meditationFactor(medF),
            manaFactor(manaF) {}

    virtual ~Clase() = default;
};

#endif
