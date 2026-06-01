#include "../includes/player.h"

#include "../includes/game_formulas.h"
#include "common/includes/core/statics.h"

#define STATE_DEAD 0

Player::Player(Inventory&& inv_, const Race& ch_race, const Clase& ch_clase, uint8_t level): inv(std::move(inv_)), ch(ch_race, ch_clase) {
    const Statics statics = ch.getStatics();
    this->hp = this->hpMax(statics.constitution);
    this->mana = this->manaMax(statics.intelligense);
    this->level = level;
    //this->dir = DOWN; /*Por default mira hacia abajo*/
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
