#include "../includes/character.h"

Character::Character(const Race& ch_race, const Clase& ch_clase): race(ch_race), clase(ch_clase) {
    this->statics.intelligense = this->clase.statics.intelligense + this->race.statics.intelligense;
    this->statics.constitution = this->clase.statics.constitution + this->race.statics.constitution;
    this->statics.strength = this->clase.statics.strength + this->race.statics.strength;
    this->statics.agility = this->clase.statics.agility + this->race.statics.agility;
}

const Statics& Character::getStatics() const { return this->statics; }

const uint16_t& Character::getHpFactorRace() { return this->race.hpFactor; }
const uint16_t& Character::getHpFactorClase() { return this->clase.hpFactor; }

const uint16_t& Character::getRecoveryFactorRace() { return this->race.recoveryFactor; }
const uint16_t& Character::getMeditationFactorClase() { return this->clase.meditationFactor; }

const uint16_t& Character::getManaFactorRace() { return this->race.manaFactor; }
const uint16_t& Character::getManaFactorClase() { return this->clase.manaFactor; }

Character::~Character() {}
