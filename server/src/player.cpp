#include "server/includes/player.h"

#include "common/includes/core/Statistics.h"
#include "server/includes/game_formulas.h"

#define STATE_DEAD 0

Player::Player(Id id, Position&& pos, Inventory&& inv_, const Race& ch_race, const Clase& ch_clase,
               uint8_t level, GameFormulas& formulas):
        CombatEntity(id, std::move(pos), 0, formulas),
        mana(0),
        exp(0),
        level(level),
        inv(std::move(inv_)),
        equipment(),
        ch(ch_race, ch_clase) {

    const Statistics statics = ch.getStatistics();

    this->max_hp = this->hpMax(statics.constitution);
    this->hp = this->max_hp;

    this->mana = this->manaMax(statics.intelligence);
}

// Constructor para un Nuevo jugador registrado desde cero
Player::Player(Id id, Position&& pos, const Race& race, const Clase& clase,
               const PlayerStateInitConfig& state_init, GameFormulas& formulas):
        CombatEntity(id, std::move(pos), 0, formulas),
        mana(0),
        exp(0),
        level(state_init.level),
        inv(state_init.golden_init, state_init.max_inventory),
        equipment(),
        ch(race, clase) {

    const Statistics statics = this->ch.getStatistics();
    this->max_hp = this->hpMax(statics.constitution);
    this->hp = this->max_hp;

    this->mana = this->manaMax(statics.intelligence);
}

uint16_t Player::hpMax(const uint16_t& constitution) {
    const uint16_t& hp_f_race = this->ch.getHpFactorRace();
    const uint16_t& hp_f_clase = this->ch.getHpFactorClase();
    return this->form.calculationMaximunHp(constitution, hp_f_race, hp_f_clase, this->level);
}

uint16_t Player::manaMax(const uint16_t& intelligense) {
    const uint16_t& mana_f_race = this->ch.getManaFactorRace();
    const uint16_t& mana_f_clase = this->ch.getManaFactorClase();
    return this->form.calculationMaximunMana(intelligense, mana_f_race, mana_f_clase, this->level);
}

Equipment& Player::getEquipment() { return this->equipment; }

uint16_t Player::calculateDamage(bool& is_critical,
                                 const std::map<TypeItem, std::unique_ptr<Item>>& info_items) {
    TypeItem weapon_type = this->equipment.getHandItem();
    const Item& item_template = *(info_items.at(weapon_type));

    auto weapon_data = dynamic_cast<const Weapon*>(&item_template);

    uint16_t min_dmg = weapon_data->minimal_damage;
    uint16_t max_dmg = weapon_data->maximun_damage;

    uint16_t strength = this->ch.getStrength();

    uint16_t damage = this->form.calculationDamage(strength, min_dmg, max_dmg, is_critical);

    return damage;
}

void Player::receiveDamage(uint16_t damage,
                           const std::map<TypeItem, std::unique_ptr<Item>>& info_items) {
    std::vector<const ItemInstance*> equipment_def = this->equipment.getEquipmentDefensive();
    uint16_t defense = this->form.calculationDefense(equipment_def, info_items);
    uint16_t net_damage = (damage > defense) ? (damage - defense) : 0;

    CombatEntity::receiveDamage(net_damage, info_items);
}

void Player::onDeath() {
    // Convertirse en fantasma
}
