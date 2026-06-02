#include "../includes/character.h"

Character::Character(const Race& ch_race, const Clase& ch_clase): race(ch_race), clase(ch_clase) {
    this->stats.intelligense =
            this->clase.statistics.intelligense + this->race.statistics.intelligense;
    this->stats.constitution =
            this->clase.statistics.constitution + this->race.statistics.constitution;
    this->stats.strength = this->clase.statistics.strength + this->race.statistics.strength;
    this->stats.agility = this->clase.statistics.agility + this->race.statistics.agility;
}

const Statistics& Character::getStatistics() const { return this->stats; }

const uint16_t& Character::getHpFactorRace() { return this->race.hp_factor; }
const uint16_t& Character::getHpFactorClase() { return this->clase.hp_factor; }

const uint16_t& Character::getRecoveryFactorRace() { return this->race.recovery_factor; }
const uint16_t& Character::getMeditationFactorClase() { return this->clase.meditation_factor; }

const uint16_t& Character::getManaFactorRace() { return this->race.mana_factor; }
const uint16_t& Character::getManaFactorClase() { return this->clase.mana_factor; }

Character::~Character() = default;
