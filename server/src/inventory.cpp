#include "server/includes/inventory.h"

#include <algorithm>

#include "server/includes/core/instances.h"
#include "server/includes/world.h"

Inventory::Inventory(): golden(0), max_inventory(0), size_current(0) {}

Inventory::Inventory(const uint32_t& golden, const uint32_t& max_inventory_,
                     const uint32_t& max_slot):
        golden(golden), max_inventory(max_inventory_), size_current(0) {

    this->slots.reserve(this->max_inventory);  // cppcheck-suppress syntaxError
    for (uint32_t i = 0; i < this->max_inventory; ++i) {
        this->slots.emplace_back(max_slot);
    }
}

bool Inventory::itemInInventory(TypeItem type_item) const {
    auto index = this->searchItemInInventory(type_item);
    return index.has_value();
}

bool Inventory::isInventoryFull() const { return this->size_current == this->max_inventory; }

bool Inventory::isInventoryEmpty() const { return this->size_current == 0; }

bool Inventory::slotEmpty(const size_t& index) const {
    if (index >= this->slots.size()) {
        return true;
    }
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

void Inventory::addItemToInventory(const ShopItem* item) {
    auto index = this->searchItemInInventory(item->type);
    if (index.has_value()) {
        this->incrementSlotInventory(index.value());
    } else {
        this->setItemInInventory(item);
    }
}

bool Inventory::setItemInTheEquipment(Equipment& equipment, size_t slot_index) {
    if (this->slotEmpty(slot_index)) {
        return false; /*SLOT VACIO*/
    }
    const auto* target_item = dynamic_cast<const ShopItem*>(this->slots[slot_index].getItem());
    if (target_item == nullptr) {
        return false;
    }

    std::unique_ptr<ItemInstance> instance = this->slots[slot_index].takeOneItem();
    if (this->slots[slot_index].isEmpty()) {
        this->size_current -= 1;
    }

    std::unique_ptr<ItemInstance> replaced_item = equipment.equipItem(std::move(instance));
    if (replaced_item && !this->setItemInstanceInInventory(replaced_item)) {
        std::unique_ptr<ItemInstance> equipped_item = equipment.equipItem(std::move(replaced_item));
        this->setItemInstanceInInventory(equipped_item);
        return false;
    }
    return true;
}

bool Inventory::removeItemFromEquipment(Equipment& equipment, size_t equipment_slot_index) {
    std::unique_ptr<ItemInstance> instance = equipment.removeItem(equipment_slot_index);
    if (!instance) {
        return false;
    }
    if (!this->setItemInstanceInInventory(instance)) {
        equipment.equipItem(std::move(instance));
        return false;
    }
    return true;
}

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

void Inventory::incrementSlotInventory(const size_t& index) {
    auto& slot = this->slots[index];
    if (!slot.isFull()) {
        slot.increase();
    }
}

void Inventory::setItemInInventory(const ShopItem* item) {
    for (auto& slot: this->slots) {
        if (slot.isEmpty()) {
            slot.setItem(std::make_unique<ItemInstance>(item));
            slot.increase();
            this->size_current += 1;
            break;
        }
    }
}

bool Inventory::setItemInstanceInInventory(std::unique_ptr<ItemInstance>& instance) {
    const auto existing_slot = this->searchItemInInventory(instance->item->type);
    if (existing_slot.has_value() && !this->slots[existing_slot.value()].isFull()) {
        this->slots[existing_slot.value()].increase();
        instance.reset();
        return true;
    }

    for (auto& slot: this->slots) {
        if (slot.isEmpty()) {
            slot.setItem(std::move(instance));
            slot.increase();
            this->size_current += 1;
            return true;
        }
    }
    return false;
}

void Inventory::removeItemFromInventory(size_t index) {
    auto& slot = this->slots[index];
    slot.decrease();
    if (slot.isEmpty()) {
        this->size_current -= 1;
    }
}

const Item* Inventory::removeItemFromInventory(TypeItem type_item) {
    if (this->isInventoryEmpty()) {
        return nullptr; /*EXCEPCION INVENTARIO VACIO*/
    }
    const auto index = this->searchItemInInventory(type_item);
    if (!index.has_value()) {
        return nullptr; /**/
    }
    const auto item = this->slots[index.has_value()].getItem();
    this->removeItemFromInventory(index.value());
    return item;
}

uint16_t Inventory::getGolden() const { return this->golden; }

void Inventory::incrementGolden(uint16_t amount) { this->golden += amount; }

void Inventory::decrementGolden(uint16_t amount) { this->golden -= amount; }

const Item* Inventory::getItemSlot(const size_t& index) { return this->slots[index].getItem(); }

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
