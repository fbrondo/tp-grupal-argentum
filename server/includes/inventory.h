#ifndef INVENTORY_H
#define INVENTORY_H

#include <memory>
#include <optional>
#include <vector>

#include "common/includes/core/position.h"
#include "common/includes/protocol.h"
#include "server/includes/core/item.h"
#include "server/includes/equipment.h"
#include "server/includes/slot.h"

class World;
class Inventory {
private:
    uint32_t golden;
    uint32_t max_inventory;
    uint32_t size_current;
    std::vector<Slot> slots;

    bool incrementSlotInventory(const size_t& index);
    bool setItemInInventory(const Item* item);
    bool setItemInstanceInInventory(std::unique_ptr<ItemInstance>& instance);
    std::optional<size_t> searchItemInInventory(TypeItem type_item) const;

public:
    Inventory(const Inventory& other) = delete;
    Inventory& operator=(const Inventory& other) = delete;
    Inventory();
    Inventory(Inventory&&) = default;
    Inventory& operator=(Inventory&&) = default;

    Inventory(const uint32_t& golden, const uint32_t& max_slots_, const uint32_t& capacity_slot);
    [[nodiscard]] bool itemInInventory(TypeItem type_item) const;
    [[nodiscard]] bool isInventoryFull() const;
    [[nodiscard]] bool isInventoryEmpty() const;
    [[nodiscard]] bool slotEmpty(const size_t& index) const;

    bool addItemToInventory(const Item* item);
    bool setItemInTheEquipment(Equipment& equipment, size_t slot_index);
    bool removeItemFromEquipment(Equipment& equipment, size_t slot_index);

    std::vector<SlotData> getSlotsData() const;
    std::vector<MsgSlot> getInventory() const;

    void dropInventory(World& world, const Position& position);
    const Item* getItemSlot(const size_t& slot_index);

    [[nodiscard]] uint16_t getGolden() const;
    void incrementGolden(uint32_t amount);
    void decrementGolden(uint32_t amount);

    void removeItemFromInventory(size_t index);
    const Item* removeItemFromInventory(TypeItem type_item);
    void reset();
};

#endif
