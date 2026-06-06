#ifndef CHARACTER_H
#define CHARACTER_H

#include <cstdint>

#include "common/includes/core/Statistics.h"
#include "common/includes/types.h"
#include "server/includes/core/clase.h"
#include "server/includes/core/race.h"

class Character {
private:
    uint8_t head;
    uint8_t body;
    Race race;
    Clase clase;
    Statistics stats;

public:
    Character(const Character&) = default;
    Character& operator=(const Character&) = default;
    Character(Character&&) = default;
    Character& operator=(Character&&) = default;
    Character(const Race& race, const Clase& clase, uint8_t head, uint8_t body);

    const Statistics& getStatistics() const;

    /*Como son enum no cuesta mucho, es mejor retornar una copia que una referencia?*/
    TypeRace getTypeRace() const;
    TypeClase getTypeClase() const;

    const uint8_t& getTypeHead() const;
    const uint8_t& getTypeBody() const;

    const uint16_t& getHpFactorRace();
    const uint16_t& getHpFactorClase();

    const uint16_t& getRecoveryFactorRace();
    const uint16_t& getMeditationFactorClase();

    const uint16_t& getManaFactorRace();
    const uint16_t& getManaFactorClase();

    ~Character();
};

#endif
