#include "../includes/equipment.h"

#include <algorithm>
#include <stdexcept>

#include "common/includes/types.h"

#define MAX_EQUIPMENT_SIZE 4
#define INDEX_HEAD 0   /*defensa*/
#define INDEX_BACK 1   /* defensa*/
#define INDEX_SHIELD 2 /*defensa - escudo*/
#define INDEX_HAND 3   /*arma u objeto magico*/

Equipment::Equipment(/* args */):
        equipment_container(MAX_EQUIPMENT_SIZE) /*4 slots todos vacios*/ {}

void Equipment::equipHandItem(ItemInstance* item_inst) {
    this->equipment_container[INDEX_HAND] = item_inst;
}

void Equipment::equipItemDefensive(ItemInstance* inst) {
    const auto item = dynamic_cast<const ShopItem*>(inst->item);
    switch (item->body_part_use) {
        case HEAD:
            this->equipment_container[INDEX_HEAD] = inst;
            break;
        case BACK:
            this->equipment_container[INDEX_BACK] = inst;
            break;
        case HAND:
            this->equipment_container[INDEX_SHIELD] = inst;
            break;
        default:
            break;
    }
}

void Equipment::equipItem(ItemInstance* instance) {
    const auto item = dynamic_cast<const ShopItem*>(instance->item);
    switch (item->classif) {
        case ITEM_ATTACK:
            this->equipHandItem(instance);
            break;
        case ITEM_DEFENSIVE:
            this->equipItemDefensive(instance);
            break;
        case ITEM_HEALING:
            this->equipHandItem(instance);
            break;
        default:
            throw std::runtime_error("Parte del cuerpo inválida para defensa.");
    }
    // ItemInstance* viejo_item = this->equipment_container[target_index];
    // this->equipment_container[target_index] = item_inst;
    // return viejo_item;
}

void Equipment::removeItem(size_t slot_index) {
    if (!this->equipment_container[slot_index]) {
        return; /*TIRAR EXCEPCION*/
    }
    this->equipment_container[slot_index] = nullptr;
}

TypeItem Equipment::getHandItem() const {
    if (this->equipment_container[INDEX_HAND]) {
        return this->equipment_container[INDEX_HAND]->item->type;
    }
    return NONE;
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

std::vector<TypeItem> Equipment::getEquipment() const {
    std::vector<TypeItem> equipment;
    for (size_t i = INDEX_HEAD; i <= INDEX_SHIELD; i++) {
        if (this->equipment_container[i]) {
            auto type = this->equipment_container[i]->item->type;
            equipment.push_back(type);
        } else {
            equipment.push_back(NONE);
        }
    }
    return equipment;
}

Equipment::~Equipment() = default;
