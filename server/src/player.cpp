#include "server/includes/player.h"

#include <cstring>
#include <sstream>
#include <vector>

#include "common/includes/core/Statistics.h"
#include "server/print.h"
#include "server/includes/game_formulas.h"
#include "server/includes/world.h"

Player::Player(const Pose& pos, Inventory&& inv_, Character&& ch_, const PlayerData& data):
        CombatEntity(pos, data), ch(std::move(ch_)) {
    this->hp = data.hp;
    this->max_hp = this->hpMax();
    this->mana = data.mana;
    this->exp = data.exp;
    this->inv = std::move(inv_);
    this->level = data.level;
    this->user.username = data.username;
    this->user.password = data.password;
    Print::imprimirCajaContenedora(data);
    /*Falta init inventario**/
}

Player::Player(const User& user_, const Pose& pose_, Character&& ch_,
               const PlayerStateInit& state_init):
        CombatEntity(pose_, 0, state_init.level), user(user_), ch(std::move(ch_)) {
    this->inv = Inventory(state_init.golden_init, state_init.max_inventory);
    this->statics = ch.getStatistics();
    this->max_hp = this->hpMax();
    this->hp = this->hpMax();
    this->mana = this->manaMax();
    this->exp = 0;
}

bool Player::hasEnoughMana(uint16_t mana_cost) const { return this->mana >= mana_cost; }

uint16_t Player::hpMax() {
    const uint16_t& hp_f_race = this->ch.getHpFactorRace();
    const uint16_t& hp_f_clase = this->ch.getHpFactorClase();
    return GameFormulas::calculationMaximunHp(this->statics.constitution, hp_f_race, hp_f_clase,
                                              this->level);
}

uint16_t Player::manaMax() {
    const uint16_t& mana_f_race = this->ch.getManaFactorRace();
    const uint16_t& mana_f_clase = this->ch.getManaFactorClase();
    return GameFormulas::calculationMaximunMana(this->statics.intelligence, mana_f_race,
                                                mana_f_clase, this->level);
}

// void Player::updatePose(Pose&& new_pos) { Entity::updatePosition(std::move(new_pos)); }

bool Player::canBuy(const ShopItem* item) const {
    uint32_t final_price = item->purchase_price;
    if (this->inv.golden < final_price) {
        return false;
    }
    if (this->inv.inventory.size() >= this->inv.max_inventory) {
        return false;
    }
    return true;
}

void Player::buyItem(const ShopItem* item, Id new_instance_id) {
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
    std::memset(data.username, 0, MAX_DATA);
    user.username.copy(data.username, MAX_DATA - 1);

    std::memset(data.password, 0, MAX_DATA);
    user.password.copy(data.password, MAX_DATA - 1);

    //std::strncpy(data.username, user.username.c_str(), MAX_DATA);
    //std::strncpy(data.password, user.password.c_str(), MAX_DATA);
    /*Pose del jugador - Posicion y direccion de mirada*/
    data.x = this->pose.position.x;
    data.y = this->pose.position.y;
    data.direction = this->pose.direct;
    /*PERSONAJE*/
    data.charact_traits.race = static_cast<uint8_t>(this->ch.getTypeRace());
    data.charact_traits.clase = static_cast<uint8_t>(this->ch.getTypeClase());
    data.charact_traits.head = static_cast<uint8_t>(this->ch.getTypeHead());
    data.charact_traits.body = static_cast<uint8_t>(this->ch.getTypeBody());
    /*Atributos actuales*/
    data.exp = this->exp;
    data.level = this->level;
    data.hp = this->hp;
    data.mana = this->mana;
    /*INVENTARIO*/
    data.golden = this->inv.golden;
    for (const auto& [id_instance, item]: this->inv.inventory) {
        ItemInstanceData item_data;
        item_data.type_item = item->type;
        item_data.x = item->pos.x;
        item_data.y = item->pos.y;
        data.inventory.push_back(item_data);
    }
    /*EQUIPO*/
    std::vector<std::tuple<Id, TypeItem>> equip = this->equipment.getEquipmentDefensive();
    for (const auto& [id, type]: equip) {
        ItemInstance* item = this->inv.inventory.at(id).get();
        ItemInstanceData item_data;
        item_data.type_item = item->type;
        // item_data.classification = item->classification;
        // item_data.body_part_use = item->body_part_use;
        item_data.x = item->pos.x;
        item_data.y = item->pos.y;
        data.equipment.push_back(item_data);
    }
    return data;
}

uint16_t Player::calculateDamage(bool& is_critical, Weapon& weapon) {
    uint16_t min_dmg = weapon.minimal_damage;
    uint16_t max_dmg = weapon.maximun_damage;
    uint16_t strength = this->statics.strength;
    uint16_t damage = GameFormulas::calculationDamage(strength, min_dmg, max_dmg, is_critical);
    return damage;
}

uint16_t Player::calculateDefense(std::vector<Defense*> info_defense) {
    return GameFormulas::calculationDefense(info_defense);
}

PlayerSnapshotData Player::getPlayerSnapshotData(const Id& player_id) {
    PlayerSnapshotData data;
    data.id = player_id;
    data.pos_x = this->pose.position.x;
    data.pos_y = this->pose.position.y;
    data.direction = this->pose.direct;
    data.hp = this->hp;
    data.max_hp = this->hpMax();
    data.mana = this->mana;
    data.max_mana = this->manaMax();
    data.body_id = this->ch.getTypeBody();
    data.head_id = this->ch.getTypeHead();
    data.raza = this->ch.getTypeRace();
    data.clase = this->ch.getTypeClase();

    return data;
    /*equipo*/
    // uint8_t weapon_id;
    // uint8_t shield_id; /*Debe ser none si no esta equipado*/
    // uint8_t armor_id;
    // uint8_t head;

    // uint8_t raza;
    // uint8_t clase;
    // uint8_t flags;
}

TypeItem Player::getHandItem() { return this->equipment.getHandItem(); }
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

void Player::updateHp(float delta) {
    if (this->hp >= this->hpMax()) {
        return;
    }
    this->hp += GameFormulas::calculationRecoveredHp(this->ch.getHpFactorRace(), delta);
    this->hp = std::min(this->hp, this->hpMax());
}

void Player::updateMana(float delta) {
    if (this->mana >= this->manaMax()) {
        return;
    }
    this->mana += GameFormulas::calculationOfManaTime(this->ch.getHpFactorRace(), delta);
    this->mana = std::min(this->mana, this->manaMax());
}

void Player::meditating(float delta) {
    if (this->mana >= this->manaMax()) {
        return;
    }
    uint16_t fact_clase = ch.getManaFactorClase();
    this->mana += GameFormulas::calculationOfManaMeditation(fact_clase, this->statics.intelligence,
                                                            delta);
    this->mana = std::min(this->mana, this->manaMax());
}

void Player::restoreAllHp() { this->hp = this->hpMax(); }

void Player::earnExperiencePoints(CombatEntity* victim, uint16_t damage) {
    this->exp += GameFormulas::calculationPointsExpAttack(damage, victim->getLevel(), this->level);
    uint16_t limit = GameFormulas::limitMoveUpToNextLevel(this->level);
    if (this->exp >= limit) {
        this->level += 1;
    }
}

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

bool Player::isNewbie() const { return this->hp <= 12; }

bool Player::isValidOpponent(Player* other) const {
    if (other == nullptr) {
        return true; /*Mi oponente es un creature*/
    }
    if (other->isNewbie() || this->isNewbie()) {
        return false;
    }
    const uint16_t max_lvl = std::max(this->level, other->level);
    const uint16_t min_lvl = std::min(this->level, other->level);
    if (max_lvl - min_lvl > 10) {
        return false;
    }
    return true;
}


void Player::onDeath(World& world) {
    /*Vacio inventario*/
    for (const auto& [id_inst, item_inv]: this->inv.inventory) {
        ItemInstance item_drop;
        item_drop.id = item_inv->id;
        item_drop.type = item_inv->type;
        item_drop.body_part_use = item_inv->body_part_use;
        item_drop.classification = item_inv->classification;
        item_drop.pos = world.findNearbyFreePosition(this->pose.position);
        world.addItemWorld(item_drop);
    }
    this->inv.inventory.clear();
    /*Perdemos el oro*/
    GoldBagInstance gold_pouche;
    gold_pouche.pos = world.findNearbyFreePosition(this->pose.position);
    world.spawnGoldOnFloor(gold_pouche);
}
