#include "../includes/equipment.h"

#include "common/includes/types.h"

#define MAX_EQUIPMENT_SIZE 4

#define INDEX_HEAD 0   /*defensa*/
#define INDEX_BACK 1   /* defensa*/
#define INDEX_SHIELD 2 /*defensa - escudo*/
#define INDEX_HAND 3   /*arma u objeto magico*/

Equipment::Equipment(/* args */):
        equipment_container(MAX_EQUIPMENT_SIZE, nullptr) /*4 slots todos vacios*/ {}

void Equipment::equipHandItem(std::unique_ptr<ItemInstace> item_inst) {
    this->equipment_container[INDEX_HAND] = std::move(item_inst);
}

void Equipment::equipItemDefensive(std::unique_ptr<ItemInstace> item_inst) {

    switch (item_inst->body_part) {
        case HEAD:
            this->equipment_container[INDEX_HEAD] = std::move(item_inst);
            break;
        case BACK:
            this->equipment_container[INDEX_BACK] = std::move(item_inst);
            break;
        case HAND:
            this->equipment_container[INDEX_SHIELD] = std::move(item_inst);
            break;
        default:
            break;
    }
}

void Equipment::equipItem(std::unique_ptr<ItemInstace> item_inst) {
    switch (item_inst->classification) {
        case DEFENSIVE:
            this->equipItemDefensive(std::move(item_inst));
            break;
        case ATTACK: /*arma de combate a mano o a distancia*/
            this->equipHandItem(std::move(item_inst));
            break;
        case HEALING: /*objeto magico*/
            this->equipHandItem(std::move(item_inst));
            break;
        default:
            break;
    }
}

std::unique_ptr<ItemInstace> Equipment::removeItem(Id id_inst_item) {
    for (auto& itemIns: this->equipment_container) {
        if (itemIns && itemIns->id_instance == id_inst_item) {
            return std::move(itemIns);
        }
    }
    return nullptr;
}

TypeItem Equipment::getHandItem() { return this->equipment_container[INDEX_HAND]->type; }

std::vector<const ItemInstace*> Equipment::getEquipmentDefensive() {
    std::vector<const ItemInstace*> equipment_def;
    for (size_t i = INDEX_HEAD; i <= INDEX_SHIELD; i++) {
        if (this->equipment_container[i]) {
            equipment_def.push_back(this->equipment_container[i].get());
        }
    }
    return equipment_def;
}

Equipment::~Equipment() {}
