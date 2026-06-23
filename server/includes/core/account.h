#pragma once
#include <cstdint>
#include <vector>

#include "server/includes/core/data.h"
#include "server/includes/slot.h"

struct Account {
    uint32_t golden{0};
    std::vector<Slot> safe_box;
    Account() = default;
    explicit Account(const uint32_t& golden);

    std::vector<SlotData> getSlotsData() const;
    void loadItemAccount(const Item* item, const uint32_t& quantity);
};
