#include "server/includes/player.h"
#include "server/includes/game_formulas.h"
#include "common/includes/core/Statistics.h"

#define STATE_DEAD 0

Player::Player(Inventory&& inv_, const Race& race, const Clase& clase, uint8_t level):
inv(std::move(inv_)), ch(race, clase) {
    const Statistics statics = ch.getStatistics();
    this->hp = this->hpMax(statics.constitution);
    this->mana = this->manaMax(statics.intelligense);
    this->level = level;
}

Player::Player(const Race& race, const Clase& clase, const PlayerStateInitConfig& state_init): ch(race,clase) {
    this->level = state_init.level;
    this->inv = Inventory(state_init.golden_init, state_init.max_inventory);
}
uint16_t Player::hpMax(const uint16_t& constitution) {
    const uint16_t& hp_f_race = this->ch.getHpFactorRace();
    const uint16_t& hp_f_clase = this->ch.getHpFactorClase();
    return this->form.calculationMaximunHp(constitution, hp_f_race, hp_f_clase, this->level);
}

uint16_t Player::manaMax(const uint16_t& intelligense) {
    const uint16_t& mana_f_race = this->ch.getManaFactorRace();
    const uint16_t& mana_f_clase = this->ch.getManaFactorClase();
    return this->form.calculationMaximunHp(intelligense, mana_f_race, mana_f_clase, this->level);
}

//const Position& Player::getCurrentPosition() const { return this->pos; }

///void Player::updatePosition(Position&& new_pos) { this->pos = std::move(new_pos); }

bool Player::isAlive() { return this->hp == STATE_DEAD; }

Player::~Player() {}
