#include "server/includes/equipment.h"

#include <algorithm>
#include <stdexcept>

#include "common/includes/types.h"
#include "server/includes/world.h"

#define MAX_EQUIPMENT_SIZE 4
#define INDEX_HEAD 0   /*defensa*/
#define INDEX_BACK 1   /* defensa*/
#define INDEX_SHIELD 2 /*defensa - escudo*/
#define INDEX_HAND 3   /*arma u objeto magico*/

Equipment::Equipment(/* args */):
        equipment_container(MAX_EQUIPMENT_SIZE) /*4 slots todos vacios*/ {}

size_t Equipment::getEquipmentIndex(const Item* item) const {
    if (item->classif != ITEM_DEFENSIVE) {
        return INDEX_HAND;
    }
    switch (item->body_part_use) {
        case HEAD:
            return INDEX_HEAD;
        case BACK:
            return INDEX_BACK;
        case HAND:
            return INDEX_SHIELD;
        default:
            throw std::runtime_error("Parte del cuerpo inválida para defensa.");
    }
}

std::unique_ptr<ItemInstance> Equipment::equipItem(std::unique_ptr<ItemInstance>&& instance) {
    // const auto item = dynamic_cast<const ShopItem*>(instance->item);
    // if (item == nullptr) {
    //     return instance;
    // }
    const size_t target_index = this->getEquipmentIndex(instance->item);
    std::unique_ptr<ItemInstance> old_item = std::move(this->equipment_container[target_index]);
    this->equipment_container[target_index] = std::move(instance);
    return old_item;
}

std::unique_ptr<ItemInstance> Equipment::removeItem(size_t slot_index) {
    if (slot_index >= this->equipment_container.size()) {
        return nullptr;
    }
    return std::move(this->equipment_container[slot_index]);
}

TypeItem Equipment::getHandItem() const {
    if (this->equipment_container[INDEX_HAND]) {
        return this->equipment_container[INDEX_HAND]->item->type;
    }
    return NONE;
}

TypeItem Equipment::getShieldItem() const {
    if (this->equipment_container[INDEX_SHIELD]) {
        return this->equipment_container[INDEX_SHIELD]->item->type;
    }
    return NONE;
}

TypeItem Equipment::getHelmetItem() const {
    if (this->equipment_container[INDEX_HEAD]) {
        return this->equipment_container[INDEX_HEAD]->item->type;
    }
    return NONE;
}

TypeItem Equipment::getArmorItem() const {
    if (this->equipment_container[INDEX_BACK]) {
        return this->equipment_container[INDEX_BACK]->item->type;
    }
    return NONE;
}

void Equipment::dropEquipment(World& world, const Position& position) {
    for (auto& equip: this->equipment_container) {
        if (equip) {
            ItemInstance item_drop(equip.get()->item);
            item_drop.position = world.findNearbyFreePosition(position);
            world.addItemWorld(item_drop);
            equip.reset(); /*lo volvemos nullpts*/
        }
    }
}

std::vector<TypeItem> Equipment::getEquipmentDefensive() const {
    std::vector<TypeItem> equipment;
    for (size_t i = INDEX_HEAD; i <= INDEX_SHIELD; i++) {
        if (this->equipment_container[i]) {
            auto type = this->equipment_container[i]->item->type;
            equipment.push_back(type);
        }
    }
    return equipment;
}

std::vector<MsgSlot> Equipment::getEquipmentSlots() const {
    std::vector<MsgSlot> equipment;
    for (size_t i = 0; i < this->equipment_container.size(); i++) {
        // if (!this->equipment_container[i]) {
        //     continue;
        // }
        MsgSlot slot;
        slot.slot_index = static_cast<uint8_t>(i);
        if (this->equipment_container[i]) {
            slot.type_item = static_cast<uint8_t>(this->equipment_container[i]->item->type);
            slot.quantity = 1;
        } else {
            slot.type_item = static_cast<uint8_t>(NONE);
            slot.quantity = 0;
        }
        equipment.push_back(slot);
    }
    return equipment;
}

Equipment::~Equipment() = default;
