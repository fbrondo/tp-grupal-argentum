#include "../includes/equipment.h"

#include <algorithm>

#include "common/includes/types.h"

#define MAX_EQUIPMENT_SIZE 4

#define INDEX_HEAD 0   /*defensa*/
#define INDEX_BACK 1   /* defensa*/
#define INDEX_SHIELD 2 /*defensa - escudo*/
#define INDEX_HAND 3   /*arma u objeto magico*/

Equipment::Equipment(/* args */):
        equipment_container(MAX_EQUIPMENT_SIZE) /*4 slots todos vacios*/ {}

void Equipment::equipHandItem(ItemInstace* item_inst) {
    this->equipment_container[INDEX_HAND] = item_inst;
}

void Equipment::equipItemDefensive(ItemInstace* item_inst) {

    switch (item_inst->body_part_use) {
        case HEAD:
            this->equipment_container[INDEX_HEAD] = item_inst;
            break;
        case BACK:
            this->equipment_container[INDEX_BACK] = item_inst;
            break;
        case HAND:
            this->equipment_container[INDEX_SHIELD] = item_inst;
            break;
        default:
            break;
    }
}

void Equipment::equipItem(ItemInstace* item_inst) {
    switch (item_inst->classification) {
        case ITEM_DEFENSIVE:
            this->equipItemDefensive(item_inst);
            break;
        case ITEM_ATTACK: /*arma de combate a mano o a distancia*/
            this->equipHandItem(item_inst);
            break;
        case ITEM_HEALING: /*objeto magico*/
            this->equipHandItem(item_inst);
            break;
        default:
            break;
    }
}

void Equipment::removeItem(Id id_inst_item) {
    for (size_t i = INDEX_HEAD; i <= INDEX_SHIELD; i++) {
        if (this->equipment_container[i] && id_inst_item == this->equipment_container[i]->id) {
            this->equipment_container[INDEX_HAND] = nullptr;
        }
    }
}

TypeItem Equipment::getHandItem() { return this->equipment_container[INDEX_HAND]->type; }

std::vector<std::tuple<Id, TypeItem>> Equipment::getEquipment() {
    std::vector<std::tuple<Id, TypeItem>> equipment;
    for (size_t i = INDEX_HEAD; i <= INDEX_SHIELD; i++) {
        if (this->equipment_container[i]) {
            auto id = this->equipment_container[i]->id;
            auto type = this->equipment_container[i]->type;
            equipment.push_back(std::make_tuple(id, type));
        }
    }
    return equipment;
}

Equipment::~Equipment() = default;
