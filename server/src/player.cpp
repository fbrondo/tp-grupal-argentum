#include "server/includes/player.h"

#include <cstring>
#include <sstream>
#include <vector>

#include "common/includes/core/Statistics.h"
#include "server/includes/game_formulas.h"

#define STATE_DEAD 0

// Player::Player(const Position& pos, Inventory&& inv_, const Race& ch_race, const Clase& ch_clase,
// uint8_t level): position(pos), inv(std::move(inv_)), ch(ch_race, ch_clase) {
//     const Statistics statics = ch.getStatistics();
//     this->hp = this->hpMax(statics.constitution);
//     this->mana = this->manaMax(statics.intelligense);
//     this->level = level;
// }

/**/
Player::Player(User&& user_, Pose&& pose_, Character&& ch_,
               const PlayerStateInitConfig& state_init):
        user(std::move(user_)), pose(pose_), ch(std::move(ch_)) {
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
void Player::updatePose(Position position, Direction direct) {
    this->pose = Pose{position, direct};
}

PlayerData Player::getPlayerData() {
    PlayerData data{};
    std::strncpy(data.username, user.username.c_str(), MAX_DATA);
    std::strncpy(data.password, user.password.c_str(), MAX_DATA);
    data.x = position.x;
    data.y = position.y;
    data.race = static_cast<uint8_t>(this->ch.getTypeRace());
    data.clase = static_cast<uint8_t>(this->ch.getTypeClase());
    data.level = this->level;
    data.hp = this->hp;
    data.mana = this->mana;

    /*INVENTARIO*/
    data.golden = this->inv.golden;
    data.number_of_items_inv = static_cast<uint32_t>(this->inv.inventory.size());
    size_t i = 0;
    for (const auto& [id_instance, item]: this->inv.inventory) {
        data.inventory[i].id = id_instance;  // inventario[i].idInstance;
        data.inventory[i].type_item = static_cast<uint8_t>(item->type);
        i += 1;
    }
    /*EQUIPO*/
    std::vector<std::tuple<Id, TypeItem>> equip = this->equipment.getEquipment();
    data.number_of_items_equip = static_cast<uint32_t>(equip.size());
    for (size_t j = 0; j < equip.size(); j++) {
        auto [id, type] = equip[j];
        data.equipment[i].id = id;
        data.equipment[i].type_item = static_cast<uint8_t>(type);
    }
    return data;
}

bool Player::isAlive() { return this->hp == STATE_DEAD; }

Player::~Player() {}
