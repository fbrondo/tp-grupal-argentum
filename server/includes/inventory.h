#ifndef INVENTORY_H
#define INVENTORY_H
#include <vector>
#include <cstdint>
#include <optional>
#include "server/includes/slot.h"
#include "server/includes/core/item.h"
class Inventory {
    private:
    uint32_t golden;
    uint32_t max_inventory;
    uint32_t size_current;
    std::vector<Slot> slots;

    public:
    Inventory(const Inventory& other) = delete;
    Inventory& operator=(const Inventory& other) = delete;
    Inventory();
    Inventory(Inventory&&) = default;
    Inventory& operator=(Inventory&&) = default;

    Inventory(const uint32_t& golden, const uint32_t& max_inventory_, const uint32_t& max_slot);
    [[nodiscard]] bool isInventoryFull() const;
    [[nodiscard]] bool isInventoryEmpty() const;
    [[nodiscard]] bool slotEmpty(const size_t& index) const;

    void incrementSlotInventory(const size_t& index);
    void setItemInInventory(const ShopItem* item);
    std::optional<size_t> itemIsInInventory(TypeItem type_item);

    std::vector<Slot>& getSlots();
    const Item* getItemSlot(const size_t& index);

    [[nodiscard]] uint16_t getGolden() const;
    void incrementGolden(uint16_t amount);
    void decrementGolden(uint16_t amount);

    void removeItemFromInventory(size_t index);
    const Item* removeItemFromInventory(TypeItem type_item);
    void reset();

};


#endif //TALLER_TP_INVENTORY_H
