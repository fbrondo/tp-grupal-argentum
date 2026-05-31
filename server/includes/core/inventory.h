#ifndef INVENTORY_H
#define INVENTORY_H

#include <map>
#include <memory>
#include <cstdint>

#include "common/includes/types.h"
#include "server/includes/core/instances.h"

struct Inventory {
    uint32_t golden;
    uint16_t max_inventory;
    std::map<Id, std::unique_ptr<ItemInstace>> inventory;

    Inventory() = default;
    Inventory(uint32_t golden_, uint16_t max_inv): golden(golden_), max_inventory(max_inv) {}
};

#endif
