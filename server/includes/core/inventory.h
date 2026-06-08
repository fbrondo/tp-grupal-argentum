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

    uint8_t getSlotOfInstance(Id instance_id) const {
        uint8_t slot_index = 0;
        
        for (const auto& [id, item] : this->inventory) {
            if (id == instance_id) {
                return slot_index;
            }
            slot_index++;
        }
        return 255; 
    }
};

#endif
