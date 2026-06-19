#ifndef INVENTORY_H
#define INVENTORY_H

#include <optional>
#include <vector>

#include "common/includes/core/position.h"
#include "common/includes/protocol.h"
#include "server/includes/core/item.h"
#include "server/includes/equipment.h"
#include "server/includes/slot.h"
// #include "server/includes/world.h"
class World;
class Inventory {
private:
    uint32_t golden;
    uint32_t max_inventory;
    uint32_t size_current;
    std::vector<Slot> slots;
    std::vector<size_t> indexs_slots_equipment;

    void incrementSlotInventory(const size_t& index);
    void setItemInInventory(const ShopItem* item);
    std::optional<size_t> searchItemInInventory(TypeItem type_item) const;

public:
    Inventory(const Inventory& other) = delete;
    Inventory& operator=(const Inventory& other) = delete;
    Inventory();
    Inventory(Inventory&&) = default;
    Inventory& operator=(Inventory&&) = default;

    Inventory(const uint32_t& golden, const uint32_t& max_inventory_, const uint32_t& max_slot);
    [[nodiscard]] bool itemInInventory(TypeItem type_item) const;
    [[nodiscard]] bool isInventoryFull() const;
    [[nodiscard]] bool isInventoryEmpty() const;
    [[nodiscard]] bool slotEmpty(const size_t& index) const;

    void addItemToInventory(const ShopItem* item);
    bool setItemInTheEquipment(Equipment& equipment, size_t slot_index);

    std::vector<SlotData> getSlotsData() const;
    std::vector<MsgSlot> getInventory() const;

    std::vector<size_t> getSlotsEquipment();

    void dropInventory(World& world, const Position& position);
    const Item* getItemSlot(const size_t& index);

    [[nodiscard]] uint16_t getGolden() const;
    void incrementGolden(uint16_t amount);
    void decrementGolden(uint16_t amount);

    void removeItemFromInventory(size_t index);
    const Item* removeItemFromInventory(TypeItem type_item);
    void reset();
};

#endif
