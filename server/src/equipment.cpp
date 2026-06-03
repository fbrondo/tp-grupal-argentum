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

std::unique_ptr<ItemInstance> Equipment::equipHandItem(std::unique_ptr<ItemInstance> item_inst) {
    std::unique_ptr<ItemInstance> old_item = std::move(this->equipment_container[INDEX_HAND]);
    this->equipment_container[INDEX_HAND] = std::move(item_inst);

    // Devolvemos el viejo (puede ser nullptr si la mano estaba vacía) para que luego se pueda meter
    // en el inventario
    return old_item;
}

std::unique_ptr<ItemInstance> Equipment::equipItemDefensive(
        std::unique_ptr<ItemInstance> item_inst) {
    std::unique_ptr<ItemInstance> old_item = nullptr;

    switch (item_inst->body_part_use) {
        case BodyPart::HEAD:
            old_item = std::move(this->equipment_container[INDEX_HEAD]);
            this->equipment_container[INDEX_HEAD] = std::move(item_inst);
            break;
        case BodyPart::BACK:
            old_item = std::move(this->equipment_container[INDEX_BACK]);
            this->equipment_container[INDEX_BACK] = std::move(item_inst);
            break;
        case BodyPart::HAND:
            old_item = std::move(this->equipment_container[INDEX_SHIELD]);
            this->equipment_container[INDEX_SHIELD] = std::move(item_inst);
            break;
        default:
            break;
    }
    return old_item;
}

void Equipment::equipItem(std::unique_ptr<ItemInstance> item_inst) {
    switch (item_inst->classification) {
        case ITEM_DEFENSIVE:
            this->equipItemDefensive(std::move(item_inst));
            break;
        case ITEM_ATTACK:  /*arma de combate a mano o a distancia*/
        case ITEM_HEALING: /*objeto magico*/
            this->equipHandItem(std::move(item_inst));
            break;
        default:
            break;
    }
}

std::unique_ptr<ItemInstance> Equipment::removeItem(Id id_inst_item) {
    auto it = std::find_if(
            equipment_container.begin(), equipment_container.end(),
            [id_inst_item](const auto& item) { return item && item->id == id_inst_item; });
    if (it != equipment_container.end())
        return std::move(*it);
    return nullptr;
}

TypeItem Equipment::getHandItem() {
    if (!this->equipment_container[INDEX_HAND]) {
        return TypeItem::NONE;
    }
    return this->equipment_container[INDEX_HAND]->type;
}

std::vector<const ItemInstance*> Equipment::getEquipmentDefensive() {
    std::vector<const ItemInstance*> equipment_def;
    for (size_t i = INDEX_HEAD; i <= INDEX_SHIELD; i++) {
        if (this->equipment_container[i]) {
            equipment_def.push_back(this->equipment_container[i].get());
        }
    }
    return equipment_def;
}

Equipment::~Equipment() = default;
