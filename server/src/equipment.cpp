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

ItemInstance* Equipment::equipItem(ItemInstance* item_inst) {
    size_t target_index = 0;

    if (item_inst->classification == ITEM_ATTACK || item_inst->classification == ITEM_HEALING) {
        target_index = INDEX_HAND;
    } else if (item_inst->classification == ITEM_DEFENSIVE) {
        switch (item_inst->body_part_use) {
            case HEAD:
                target_index = INDEX_HEAD;
                break;
            case BACK:
                target_index = INDEX_BACK;
                break;
            case HAND:
                target_index = INDEX_SHIELD;
                break;
            default:
                throw std::runtime_error("Parte del cuerpo inválida para defensa.");
        }
    }

    ItemInstance* viejo_item = this->equipment_container[target_index];
    this->equipment_container[target_index] = item_inst;
    return viejo_item;
}

ItemInstance* Equipment::removeItem(Id id_inst_item) {
    for (size_t i = 0; i < MAX_EQUIPMENT_SIZE; i++) {
        if (this->equipment_container[i] && this->equipment_container[i]->id == id_inst_item) {
            ItemInstance* item_encontrado = this->equipment_container[i];

            this->equipment_container[i] = nullptr;
            return item_encontrado;
        }
    }
    return nullptr;
}

TypeItem Equipment::getHandItem() const {
    if (this->equipment_container[INDEX_HAND]) {
        return this->equipment_container[INDEX_HAND]->type;
    }
    return NONE;
}

std::vector<TypeItem> Equipment::getEquipmentDefensive() {
    std::vector<TypeItem> equipment;
    for (size_t i = INDEX_HEAD; i <= INDEX_SHIELD; i++) {
        if (this->equipment_container[i]) {
            auto type = this->equipment_container[i]->item->type;
            equipment.push_back(type);
        }
    }
    return equipment;
}

Equipment::~Equipment() = default;
