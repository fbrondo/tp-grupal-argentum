#include "server/includes/inventory.h"

#include <algorithm>

#include "server/includes/core/instances.h"
#include "server/includes/world.h"
#include "server/print.h"

Inventory::Inventory(): golden(0), max_inventory(0), size_current(0) {}

Inventory::Inventory(const uint32_t& golden, const uint32_t& max_slots_,
                     const uint32_t& capacity_slot):
        golden(golden), size_current(0) {
    this->max_inventory = max_slots_ * capacity_slot;
    this->slots.reserve(max_slots_);  // cppcheck-suppress syntaxError
    for (uint32_t i = 0; i < max_slots_; ++i) {
        this->slots.emplace_back(capacity_slot);
    }
}

bool Inventory::itemInInventory(TypeItem type_item) const {
    auto index = this->searchItemInInventory(type_item);
    return index.has_value();
}

bool Inventory::isInventoryFull() const { return this->size_current == this->max_inventory; }

bool Inventory::isInventoryEmpty() const { return this->size_current == 0; }

bool Inventory::slotEmpty(const size_t& index) const {
    const auto& slot = this->slots[index];
    return slot.isEmpty();
}

std::optional<size_t> Inventory::searchItemInInventory(TypeItem type_item) const {
    for (size_t i = 0; i < this->slots.size(); i++) {
        if (!this->slots[i].isEmpty() && this->slots[i].getTypeItem() == type_item) {
            return i;
        }
    }
    return std::nullopt;
}

bool Inventory::setItemInTheEquipment(Equipment& equipment, size_t slot_index) {
    if (this->slotEmpty(slot_index)) {
        return false;
    }
    std::unique_ptr<ItemInstance> instance = this->slots[slot_index].takeOneItem();
    if (this->slots[slot_index].isEmpty()) {
        this->size_current -= 1;
    }
    std::unique_ptr<ItemInstance> replaced_item = equipment.equipItem(std::move(instance));
    if (replaced_item && !this->addItemToInventory(replaced_item->item)) {
        std::unique_ptr<ItemInstance> equipped_item = equipment.equipItem(std::move(replaced_item));
        this->setItemInInventory(equipped_item->item);
        return false;
    }
    return true;
    // if (replaced_item && !this->setItemInstanceInInventory(replaced_item)) {
    // std::unique_ptr<ItemInstance> equipped_item = equipment.equipItem(std::move(replaced_item));
    // this->setItemInstanceInInventory(equipped_item);
    //   return false;
    //}
    // return true;
}
// const auto target_item = dynamic_cast<const ShopItem*>(this->slots[slot_index].getItem());
// if (target_item == nullptr) {
//     return false;
// }
bool Inventory::removeItemFromEquipment(Equipment& equipment, size_t equipment_slot_index) {
    if (this->isInventoryFull()) { /*Fijarse primero si el inventario esta lleno*/
        return false;
    }
    std::unique_ptr<ItemInstance> instance = equipment.removeItem(equipment_slot_index);
    if (!instance) {
        return false;
    }
    return this->setItemInInventory(instance->item);
    // if (!this->setItemInstanceInInventory(instance->item)) {
    //     equipment.equipItem(std::move(instance));
    //     return false;
    // }
}

void Inventory::dropInventory(World& world, const Position& position) {
    for (auto& slot: slots) {
        while (!slot.isEmpty()) {
            ItemInstance item_drop(slot.getItem());
            item_drop.position = world.findNearbyFreePosition(position);
            world.addItemWorld(item_drop);
            slot.decrease();
        }
    }
    this->reset();
}

bool Inventory::incrementSlotInventory(const size_t& index) {
    auto& slot = this->slots[index];
    if (!slot.isFull()) {
        slot.increase();
        this->size_current += 1;
        return true;
    } else { /*Buscamos uno vacio*/
        return this->setItemInInventory(slot.getItem());
    }
}

bool Inventory::setItemInInventory(const Item* item) {
    for (auto& slot: this->slots) {
        if (slot.isEmpty()) {
            slot.setItem(std::make_unique<ItemInstance>(item));
            slot.increase();
            this->size_current += 1;
            return true;
        }
    }
    return false;
}


bool Inventory::setItemInstanceInInventory(std::unique_ptr<ItemInstance>& instance) {
    const auto index_slot = this->searchItemInInventory(instance->item->type);
    if (index_slot.has_value() && !this->slots[index_slot.value()].isFull()) {
        this->slots[index_slot.value()].increase();  // instance.reset();
        return true;
    }
    return this->setItemInInventory(instance->item);
}

void Inventory::removeItemFromInventory(size_t index) {
    auto& slot = this->slots[index];
    slot.decrease();
    this->size_current -= 1;
}

const Item* Inventory::removeItemFromInventory(TypeItem type_item) {
    if (this->isInventoryEmpty()) {
        return nullptr;
    }
    const auto index = this->searchItemInInventory(type_item);
    if (!index.has_value()) {
        return nullptr;
    }
    const auto item = this->slots[index.value()].getItem();
    this->removeItemFromInventory(index.value());
    return item;
}

uint16_t Inventory::getGolden() const { return this->golden; }

void Inventory::incrementGolden(uint32_t amount) { this->golden += amount; }

void Inventory::decrementGolden(uint32_t amount) { this->golden -= amount; }

bool Inventory::addItemToInventory(const Item* item) {
    auto index = this->searchItemInInventory(item->type);
    if (index.has_value()) {
        return this->incrementSlotInventory(index.value());
    } else {
        return this->setItemInInventory(item);
    }
}

const Item* Inventory::getItemSlot(const size_t& index) { return this->slots[index].getItem(); }

std::vector<SlotData> Inventory::getSlotsData() const {
    std::vector<SlotData> slots_data;
    for (size_t i = 0; i < this->slots.size(); i++) {
        if (!slots[i].isEmpty()) {
            const auto type_item = static_cast<uint8_t>(this->slots[i].getTypeItem());
            SlotData data(type_item, i, slots[i].getQuantity());
            slots_data.emplace_back(data);
        }
    }
    return slots_data;
}

std::vector<MsgSlot> Inventory::getInventory() const {
    std::vector<MsgSlot> inventory;
    for (size_t i = 0; i < this->slots.size(); i++) {
        MsgSlot s;
        s.type_item = static_cast<uint8_t>(this->slots[i].getTypeItem());
        s.slot_index = static_cast<uint8_t>(i);
        s.quantity = this->slots[i].getQuantity();
        inventory.emplace_back(s);
    }
    return inventory;
}

void Inventory::reset() { this->size_current = 0; }
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
