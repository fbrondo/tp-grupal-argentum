#ifndef REGION_H
#define REGION_H

#include <string>
#include <vector>
#include <cstdint>
#include <optional>
#include "common/includes/map/tile.h"

struct RegionWorld {
    Region type;
    std::vector<std::string> npc_types;
    virtual ~RegionWorld() = default;
};

struct WildRegion : RegionWorld {
    uint16_t max_creatures;
    std::optional<uint16_t> min_treasure;
    std::optional<uint16_t> max_treasure;
};

struct SafeRegion : RegionWorld {
    uint16_t merchants;
    uint16_t priests;
    uint16_t bankers;
};

#endif //REGION_H
