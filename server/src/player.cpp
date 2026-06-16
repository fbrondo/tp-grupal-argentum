#include "server/includes/player.h"

#include <cstring>
#include <vector>
#include "common/includes/core/Statistics.h"
#include "server/print.h"
#include "server/includes/slot.h"
#include "server/includes/game_formulas.h"
#include "server/includes/world.h"

Player::Player(const Pose& pos, Inventory&& inv_, Character&& ch_, const PlayerData& data):
        CombatEntity(pos, data), ch(std::move(ch_)), inv(std::move(inv_)) {
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
    this->inv = Inventory(state_init.golden_init, state_init.max_inventory, state_init.max_inventory);
    this->statics = ch.getStatistics();
    this->max_hp = this->hpMax();
    this->hp = this->hpMax();
    this->mana = this->manaMax();
    this->exp = 0;
}

// uint8_t Player::getSlotOfInstance(Id instance_id) const {
//     return this->inv.getSlotOfInstance(instance_id);
// }

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



bool Player::canBuy(uint16_t price_item) const {
    if (this->inv.getGolden() < price_item) {
        return false; /*TIRAR EXCEPCION NO POSIBLE COMPRA*/
    }
    if (this->inv.isInventoryFull()) {
        return false;  /*TIRAR EXCEPCION INVENTARIO*/
    }
    return true;
}

// std::optional<size_t> Player::itemIsInInventory(TypeItem type_item) const {
//     for (size_t i = 0; i < this->inv.slots.size(); i++) {
//         if (!this->inv.slots[i].isEmpty() && this->inv.slots[i]item->type == type_item) {
//             return i;
//         }
//     }
//     return std::nullopt;
// }


void Player::addItemToInventory(const ItemInstance &instance) {
    if (this->inv.isInventoryFull()) {
        return;
    }
    auto index = this->inv.itemIsInInventory(instance.item->type);
    if (index.has_value()) {
        this->inv.incrementSlotInventory(index.value());
    } else {
        const auto item = dynamic_cast<const ShopItem*>(instance.item);
        this->inv.setItemInInventory(item);
    }
}


void Player::addItemToInventory(const GoldBagInstance &instance) {
    this->inv.incrementGolden(instance.amount);
}

void Player::addItemToInventory(const TreasureInstance &instance) {
    this->inv.incrementGolden(instance.amount_golden);
    for (auto& item_inst: instance.items) {
        if (this->inv.isInventoryFull()) {
            return;
        }
        auto index = this->inv.itemIsInInventory(item_inst.item->type);
        if (index.has_value()) {
            this->inv.incrementSlotInventory(index.value());
        } else {
            const auto item = dynamic_cast<const ShopItem*>(item_inst.item);
            this->inv.setItemInInventory(item);
        }
    }
}

void Player::buyItem(const ShopItem* item) {
    const uint16_t price = item->purchase_price;
    if (!this->canBuy(price)) {
        return;
    }
    this->inv.decrementGolden(price);
    auto index = this->inv.itemIsInInventory(item->type);
    if (index.has_value()) {
        this->inv.incrementSlotInventory(index.value());
    } else {
        this->inv.setItemInInventory(item);
    }
}

void Player::sellItem(TypeItem type_item, uint32_t sell_price) {
    auto index = this->inv.itemIsInInventory(type_item);
    if (!index.has_value()) {
        return;
    }
    this->inv.incrementGolden(sell_price);
    this->inv.removeItemFromInventory(index.has_value());

}

void Player::dropItem(size_t index_slot, World& world) {
    if (this->inv.slotEmpty(index_slot)) {
        return;
    }
    const auto item = this->inv.getItemSlot(index_slot);
    ItemInstance instance(item);
    instance.position = world.findNearbyFreePosition(this->pose.position);
    world.addItemWorld(instance);
    this->inv.removeItemFromInventory(index_slot);
}


PlayerData Player::getPlayerData() {
    PlayerData data{};
    std::memset(data.username, 0, MAX_DATA);
    user.username.copy(data.username, MAX_DATA - 1);

    std::memset(data.password, 0, MAX_DATA);
    user.password.copy(data.password, MAX_DATA - 1);

    /*Pose del jugador - Posicion y direccion de mirada*/
    data.position = this->pose.position;
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
    data.golden = this->inv.getGolden();
    const auto& slots = this->inv.getSlots();
    for (uint32_t i = 0; i < slots.size(); i++) {
        if (!slots[i].isEmpty()) {
            SlotData slot{};
            slot.index = i;
            slot.item.type_item = static_cast<uint8_t>(slots[i].getTypeItem());
            slot.quantity = slots[i].getQuantity();
            data.inventory.push_back(slot);
        }
    }
    /*EQUIPO*/
    std::vector<TypeItem> equip = this->equipment.getEquipmentDefensive();
    for (const auto& type: equip) {
        ItemInstanceData item_data;
        for (uint32_t idx = 0; idx < slots.size(); idx++) {
            if (!slots[idx].isEmpty() &&  slots[idx].getTypeItem() == type) {
                data.equipment.push_back(idx);
                break;
            }
        }
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
// std::vector<TypeItem> Player::getEquipment() {
//     /*Esto no cuesta nada, lo maximo que puedo llegar a tener en el equip son 4 elementos -> O(1)*/
//     std::vector<std::tuple<Id, TypeItem>> equip = this->equipment.getEquipmentDefensive();
//     std::vector<TypeItem> equipTypes;
//     for (size_t j = 0; j < equip.size(); j++) {
//         auto [_, type] = equip[j];
//         equipTypes.push_back(type);
//     }
//     return equipTypes;
// }

Inventory& Player::getInventory() { return this->inv; }
//
// ItemInstance* Player::getItemInstance(Id instance_id) {
//     auto it_inv = this->inv.inventory.find(instance_id);
//     if (it_inv != this->inv.inventory.end()) {
//         return it_inv->second.get();
//     }
//     // Si no esta en el inv busco en el banco
//     auto it_bank = this->bank_account.safe_box.find(instance_id);
//     if (it_bank != this->bank_account.safe_box.end()) {
//         return it_bank->second.get();
//     }
//     return nullptr;
// }

//uint32_t Player::getInventorySize() const { return this->inv.inventory.size(); }

//uint32_t Player::getMaxInventorySize() const { return this->inv.max_inventory; }

//uint32_t Player::getInventoryGold() const { return this->inv.golden; }

//uint32_t Player::getBankGold() const { return this->bank_account.gold; }

//void Player::increaseInventoryGold(uint32_t amount) { this->inv.golden += amount; }

//void Player::decreaseInventoryGold(uint32_t amount) { this->inv.golden -= amount; }

// void Player::increaseBankGold(uint32_t amount) { this->bank_account.gold += amount; }
//
// void Player::decreaseBankGold(uint32_t amount) { this->bank_account.gold -= amount; }
//
// void Player::addItemToInventory(std::unique_ptr<ItemInstance> item) {
//     this->inv.inventory.emplace(item->id, std::move(item));
// }

// std::unique_ptr<ItemInstance> Player::removeItemFromInventory(Id instance_id) {
//     auto node = this->inv.inventory.extract(instance_id);
//
//     return std::move(node.mapped());
// }
//
// size_t Player::getBankSize() const { return this->bank_account.safe_box.size(); }
//
// size_t Player::getMaxBankSize() const { return this->bank_account.max_size; }
//
// bool Player::hasItemInBank(Id instance_id) const {
//     return this->bank_account.safe_box.count(instance_id) > 0;
// }
//
// void Player::addItemToBank(std::unique_ptr<ItemInstance> item) {
//     Id instance_id = item->id;
//     this->bank_account.safe_box.emplace(instance_id, std::move(item));
// }
//
// std::unique_ptr<ItemInstance> Player::removeItemFromBank(Id instance_id) {
//     auto node = this->bank_account.safe_box.extract(instance_id);
//     return std::move(node.mapped());
// }

void Player::teleportTo(const Position& pos) { this->pose.position = pos; }

std::string Player::getUsername() {
    return this->user.username;
}

const Item * Player::removeItemInventory(TypeItem type_item) {
    if (this->inv.isInventoryEmpty()) {
        return nullptr;
    }
    const auto index = this->inv.itemIsInInventory(type_item);
    if (index.has_value()) {
        const Item* item = this->inv.getItemSlot(index.value());
        this->inv.removeItemFromInventory(index.value());
        return item;
    }
   return nullptr;
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

void Player::restoreAllMana() { this->mana = this->manaMax(); }

void Player::earnExperiencePoints(CombatEntity* victim, uint16_t damage) {
    this->exp += GameFormulas::calculationPointsExpAttack(damage, victim->getLevel(), this->level);
    uint16_t limit = GameFormulas::limitMoveUpToNextLevel(this->level);
    if (this->exp >= limit) {
        this->level += 1;
    }
}

void Player::restoreHp(uint16_t hp) {
    this->hp = std::min(static_cast<uint16_t>(this->hp + hp), this->hpMax());
}

// void Player::restoreMana(uint16_t mana) {
//     this->mana = std::min(static_cast<uint16_t>(this->mana + mana), this->manaMax());
// }
//
// std::vector<MsgItemInfo> Player::getBankItemsInfo() const {
//     std::vector<MsgItemInfo> info_vector;
//
//     info_vector.reserve(this->bank_account.safe_box.size());
//
//     for (auto& [id_clave, item_ptr]: this->bank_account.safe_box) {
//         MsgItemInfo msg;
//         msg.instance_id = id_clave;
//         msg.item_type = static_cast<uint8_t>(item_ptr->type);
//
//         info_vector.push_back(msg);
//     }
//
//     return info_vector;
// }
//
// void Player::equipItem(Id instance_id) {
//     auto it = this->inv.inventory.find(instance_id);
//     if (it == this->inv.inventory.end()) {
//         throw std::runtime_error("No tienes ese ítem en tu inventario.");
//     }
//
//     ItemInstance* item_to_equip = it->second.get();
//
//     ItemInstance* old_item = this->equipment.equipItem(item_to_equip);
//
//     item_to_equip->is_equipped = true;
//
//     if (old_item) {
//         old_item->is_equipped = false;
//     }
// }
//
// void Player::unequipItem(Id instance_id) {
//     if (this->inv.inventory.size() >= this->inv.max_inventory) {
//         throw std::runtime_error("No tienes espacio suficiente en el inventario.");
//     }
//
//     ItemInstance* item_to_unequip = this->equipment.removeItem(instance_id);
//     item_to_unequip->is_equipped = false;
// }
//
// void Player::useItem(Id instance_id) {
//     auto it = this->inv.inventory.find(instance_id);
//     if (it == this->inv.inventory.end()) {
//         throw std::runtime_error("No tienes ese objeto.");
//     }
//
//     ItemInstance* item_inst = it->second.get();
//     if (item_inst->is_equipped) {
//         throw std::runtime_error("No puedes usar un objeto equipado.");
//     }
//     bool se_consume = const_cast<Item*>(item_inst->item)->use(*this);
//
//     if (se_consume) {
//         this->inv.inventory.erase(it);
//     }
// }




void Player::onDeath(World& world) {
    auto& slots = this->inv.getSlots();
    for (auto& slot : slots) {
        while (!slot.isEmpty()) {
            ItemInstance item_drop(slot.getItem());
            item_drop.position = world.findNearbyFreePosition(this->pose.position);
            world.addItemWorld(item_drop);
            slot.decrease();
        }
    }
    this->inv.reset();

    /* Perdemos el oro */
    GoldBagInstance gold_pouche;
    gold_pouche.amount = this->inv.getGolden();
    gold_pouche.position = world.findNearbyFreePosition(this->pose.position);
    world.spawnGoldOnFloor(gold_pouche);
    this->inv.decrementGolden(gold_pouche.amount);
}
