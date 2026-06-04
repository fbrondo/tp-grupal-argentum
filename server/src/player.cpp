#include "server/includes/player.h"

#include <cstring>
#include <sstream>
#include <vector>

#include "common/includes/core/Statistics.h"
#include "server/includes/game_formulas.h"

#define STATE_DEAD 0

// Player::Player(Pose&& pose, Inventory&& inv_, const Race& ch_race, const Clase& ch_clase, uint8_t
// level):
//         CombatEntity(std::move(pose), 0),
//         inv(std::move(inv_)),
//         ch(ch_race, ch_clase) {
//
//     this->statics = ch.getStatistics();
//     this->max_hp = this->hpMax();
//     this->hp = this->hpMax();;
//     this->mana = this->manaMax();
// }

/**/
Player::Player(User&& user_, Pose&& pose_, Character&& ch_,
               const PlayerStateInitConfig& state_init):
        CombatEntity(std::move(pose_), 0), user(std::move(user_)), ch(std::move(ch_)) {
    this->level = state_init.level;
    this->inv = Inventory(state_init.golden_init, state_init.max_inventory);
    this->statics = ch.getStatistics();
    CombatEntity::max_hp = this->hpMax();
    CombatEntity::hp = this->hpMax();
    this->mana = this->manaMax();
}

bool Player::hasEnoughMana(uint16_t mana_cost) const { return this->mana >= mana_cost; }

uint16_t Player::hpMax() {
    const uint16_t& hp_f_race = this->ch.getHpFactorRace();
    const uint16_t& hp_f_clase = this->ch.getHpFactorClase();
    return GameFormulas::calculationMaximunHp(this->statics.constitution, hp_f_race, hp_f_clase,
                                              this->level);
}

void Player::updatePose(Position position, Direction direct) {
    Entity::updatePosition(Pose{position, direct});
}

PlayerData Player::getPlayerData() {
    PlayerData data{};
    std::strncpy(data.username, user.username.c_str(), MAX_DATA);
    std::strncpy(data.password, user.password.c_str(), MAX_DATA);

    /*Pose del jugador - Posicion y direccion de mirada*/
    data.x = Entity::pose.position.x;
    data.y = Entity::pose.position.y;
    data.direction = Entity::pose.direct;

    /*PERSONAJE*/
    data.charact_traits.race = static_cast<uint8_t>(this->ch.getTypeRace());
    data.charact_traits.clase = static_cast<uint8_t>(this->ch.getTypeClase());
    data.charact_traits.head = static_cast<uint8_t>(this->ch.getTypeHead());
    data.charact_traits.body = static_cast<uint8_t>(this->ch.getTypeBody());

    /*Atributos actuales*/
    data.level = this->level;
    data.hp = CombatEntity::hp;
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
    std::vector<std::tuple<Id, TypeItem>> equip = this->equipment.getEquipmentDefensive();
    data.number_of_items_equip = static_cast<uint32_t>(equip.size());
    for (size_t j = 0; j < equip.size(); j++) {
        auto [id, type] = equip[j];
        data.equipment[i].id = id;
        data.equipment[i].type_item = static_cast<uint8_t>(type);
    }
    return data;
}

uint16_t Player::calculateDamage(bool& is_critical, Weapon& weapon) {
    // TypeItem weapon_type = this->equipment.getHandItem();
    // const Item& item_template = *(info_items.at(weapon_type));
    // auto weapon_data = dynamic_cast<const Weapon*>(&item_template);
    uint16_t min_dmg = weapon.minimal_damage;  // weapon_data->minimal_damage;
    uint16_t max_dmg = weapon.maximun_damage;  // weapon_data->maximun_damage;
    uint16_t strength = this->statics.strength;
    uint16_t damage = GameFormulas::calculationDamage(strength, min_dmg, max_dmg, is_critical);
    return damage;
}

uint16_t Player::calculateDefense(std::vector<Defense*> info_defense) {
    return GameFormulas::calculationDefense(info_defense);
}

std::vector<TypeItem> Player::getEquipment() {
    /*Esto no cuesta nada, lo maximo que puedo llegar a tener en el equip son 4 elementos -> O(1)*/
    std::vector<std::tuple<Id, TypeItem>> equip = this->equipment.getEquipmentDefensive();
    std::vector<TypeItem> equipTypes;
    for (size_t j = 0; j < equip.size(); j++) {
        auto [_, type] = equip[j];
        equipTypes.push_back(type);
    }
    return equipTypes;
}

void Player::onDeath() {
    // Convertirse en fantasma
}
