#include "server/includes/character.h"

Character::Character(const Race& race, const Clase& clase, uint16_t head_, uint16_t body_):
        race(race), clase(clase) {
    this->head = head_;
    this->body = body_;
    this->stats.intelligence =
            this->clase.statistics.intelligence + this->race.statistics.intelligence;
    this->stats.constitution =
            this->clase.statistics.constitution + this->race.statistics.constitution;
    this->stats.strength = this->clase.statistics.strength + this->race.statistics.strength;
    this->stats.agility = this->clase.statistics.agility + this->race.statistics.agility;
}

const Statistics& Character::getStatistics() const { return this->stats; }


TypeRace Character::getTypeRace() const { return this->race.type; }
TypeClase Character::getTypeClase() const { return this->clase.type; }

const uint16_t& Character::getTypeHead() const { return this->head; }
const uint16_t& Character::getTypeBody() const { return this->body; }

const uint16_t& Character::getHpFactorRace() { return this->race.hp_factor; }
const uint16_t& Character::getHpFactorClase() { return this->clase.hp_factor; }

const uint16_t& Character::getRecoveryFactorRace() { return this->race.recovery_factor; }
const uint16_t& Character::getMeditationFactorClase() { return this->clase.meditation_factor; }

const uint16_t& Character::getManaFactorRace() { return this->race.mana_factor; }
const uint16_t& Character::getManaFactorClase() { return this->clase.mana_factor; }

Character::~Character() = default;
