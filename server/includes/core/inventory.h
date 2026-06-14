#ifndef INVENTORY_H
#define INVENTORY_H

#include <cstdint>
#include <map>
#include <memory>

#include "common/includes/types.h"
#include "server/includes/core/instances.h"

struct Inventory {
    uint32_t golden;
    uint32_t max_inventory;
    std::map<Id, std::unique_ptr<ItemInstance>> inventory;

    Inventory(): golden(0), max_inventory(0) {}
    Inventory(uint32_t golden_, uint32_t max_inv): golden(golden_), max_inventory(max_inv) {}
};

#endif
