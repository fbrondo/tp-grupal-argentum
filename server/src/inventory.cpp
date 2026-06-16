#include "server/includes/inventory.h"
#include "server/includes/core/instances.h"

Inventory::Inventory():golden(0), max_inventory(0), size_current(0){
}

Inventory::Inventory(const uint32_t& golden, const uint32_t &max_inventory_, const uint32_t &max_slot):
golden(golden),
max_inventory(max_inventory_),
size_current(0){
    this->slots.reserve(max_inventory);
    for (uint32_t i = 0; i < max_inventory; ++i) {
        slots.emplace_back(max_slot);
    }
}

bool Inventory::isInventoryFull() const {
    return this->size_current == this->max_inventory;
}

bool Inventory::isInventoryEmpty() const {
    return this->size_current == 0;
}

bool Inventory::slotEmpty(const size_t& index) const {
    const auto& slot = this->slots[index];
    return slot.isEmpty();
}

std::optional<size_t> Inventory::itemIsInInventory(TypeItem type_item) {
    for (size_t i = 0; i < this->slots.size(); i++) {
        if (!this->slots[i].isEmpty() && this->slots[i].getTypeItem() == type_item) {
            return i;
        }
    }
    return std::nullopt;
}

void Inventory::incrementSlotInventory(const size_t& index) {
    auto& slot = this->slots[index];
    slot.increase();
    if (slot.isFull()) {
        this->size_current+=1;
    }
}

void Inventory::setItemInInventory(const ShopItem *item) {
    for (auto& slot: this->slots) {
        if (slot.isEmpty()) {
            slot.setItem(std::make_unique<ItemInstance>(item));
            slot.increase();
            break;
        }
    }
}

void Inventory::removeItemFromInventory(size_t index) {
    auto& slot = this->slots[index];
    slot.decrease();
    if (slot.isEmpty()) {
        this->size_current-=1;
    }
}

const Item* Inventory::removeItemFromInventory(TypeItem type_item) {
    if (this->isInventoryEmpty()) {
        return nullptr; /*EXCEPCION INVENTARIO VACIO*/
    }
    const auto index = this->itemIsInInventory(type_item);
    if (!index.has_value()) {
      return nullptr; /**/
    }
    const auto item= this->slots[index.has_value()].getItem();
    this->removeItemFromInventory(index.value());
    return item;
}

uint16_t Inventory::getGolden() const {
    return this->golden;
}

void Inventory::incrementGolden(uint16_t amount) {
    this->golden += amount;
}

void Inventory::decrementGolden(uint16_t amount) {
    this->golden -= amount;
}

std::vector<Slot> & Inventory::getSlots(){
    return this->slots;
}

const Item* Inventory::getItemSlot(const size_t &index) {
    return this->slots[index].getItem();
}

void Inventory::reset() {
    this->size_current = 0;
}
// uint8_t getSlotOfInstance(Id instance_id) const {
//     uint8_t slot_index = 0;
//
//     for (const auto& [id, item] : this->inventory) {
//         if (id == instance_id) {
//             return slot_index;
//         }
//         slot_index++;
//     }
//     return 255;
// }
