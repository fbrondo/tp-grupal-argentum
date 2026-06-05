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

bool Player::canBuy(const Item* item) const {
    uint32_t final_price = item->purchase_price;
    if (this->inv.golden < final_price) {
        return false;
    }

    if (this->inv.inventory.size() >= this->inv.max_inventory) {
        return false;
    }

    return true;
}

void Player::buyItem(const Item* item, Id new_instance_id) {
    uint32_t final_price = item->purchase_price;
    this->inv.golden -= final_price;

    auto new_instance = std::make_unique<ItemInstance>(new_instance_id, item->type, item->classif,
                                                       item->body_part_use, this->pose.position);

    this->inv.inventory.emplace(new_instance_id, std::move(new_instance));
}

bool Player::canSell(const Id instance_id) const {
    return this->inv.inventory.find(instance_id) != this->inv.inventory.end();
}

void Player::sellItem(Id instance_id, uint32_t sell_price) {
    this->inv.golden += sell_price;
    this->inv.inventory.erase(instance_id);
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

Inventory& Player::getInventory() { return this->inv; }

ItemInstance* Player::getItemInstance(Id instance_id) {
    return this->inv.inventory.at(instance_id).get();
}

Pose Player::getPose() const { return this->pose; }

uint32_t Player::getInventorySize() const { return this->inv.inventory.size(); }

uint32_t Player::getMaxInventorySize() const { return this->inv.max_inventory; }

uint32_t Player::getInventoryGold() const { return this->inv.golden; }

uint32_t Player::getBankGold() const { return this->bank_account.gold; }

void Player::increaseInventoryGold(uint32_t amount) { this->inv.golden += amount; }

void Player::decreaseInventoryGold(uint32_t amount) { this->inv.golden -= amount; }

void Player::increaseBankGold(uint32_t amount) { this->bank_account.gold += amount; }

void Player::decreaseBankGold(uint32_t amount) { this->bank_account.gold -= amount; }

void Player::addItemToInventory(std::unique_ptr<ItemInstance> item) {
    this->inv.inventory.emplace(item->id, std::move(item));
}

std::unique_ptr<ItemInstance> Player::removeItemFromInventory(Id instance_id) {
    auto node = this->inv.inventory.extract(instance_id);

    return std::move(node.mapped());
}

size_t Player::getBankSize() const { return this->bank_account.safe_box.size(); }

size_t Player::getMaxBankSize() const { return this->bank_account.max_size; }

bool Player::hasItemInBank(Id instance_id) const {
    return this->bank_account.safe_box.count(instance_id) > 0;
}

void Player::addItemToBank(std::unique_ptr<ItemInstance> item) {
    Id instance_id = item->id;
    this->bank_account.safe_box.emplace(instance_id, std::move(item));
}

std::unique_ptr<ItemInstance> Player::removeItemFromBank(Id instance_id) {
    auto node = this->bank_account.safe_box.extract(instance_id);
    return std::move(node.mapped());
}

bool Player::isMeditating() const { return this->is_meditating; }

void Player::toggleMeditation() { this->is_meditating = !this->is_meditating; }

void Player::breakMeditation() { this->is_meditating = false; }

void Player::restoreAllHp() { this->hp = this->hpMax(); }

void Player::restoreAllMana() { this->mana = this->manaMax(); }

std::vector<MsgItemInfo> Player::getBankItemsInfo() const {
    std::vector<MsgItemInfo> info_vector;

    info_vector.reserve(this->bank_account.safe_box.size());

    for (auto& [id_clave, item_ptr]: this->bank_account.safe_box) {
        MsgItemInfo msg;
        msg.instance_id = id_clave;
        msg.item_type = static_cast<uint8_t>(item_ptr->type);

        info_vector.push_back(msg);
    }

    return info_vector;
}


void Player::onDeath() {
    // Convertirse en fantasma
}
