#ifndef CHARACTER_H
#define CHARACTER_H

#include "common/includes/core/Statistics.h"
#include "server/includes/core/clase.h"
#include "server/includes/core/race.h"

class Character {
private:
    const Race& race;
    const Clase& clase;
    Statistics stats;

public:
    Character(const Race& ch_race, const Clase& ch_clase);

    /*statics*/
    const Statistics& getStatistics() const;

    uint16_t getStrength() const; 
    uint16_t getAgility() const;
    uint16_t getIntelligence() const;
    uint16_t getConstitution() const;

    const uint16_t& getHpFactorRace();
    const uint16_t& getHpFactorClase();

    const uint16_t& getRecoveryFactorRace();
    const uint16_t& getMeditationFactorClase();

    const uint16_t& getManaFactorRace();
    const uint16_t& getManaFactorClase();

    ~Character();
};

#endif
