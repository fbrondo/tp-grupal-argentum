#ifndef REGION_H
#define REGION_H

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include "common/includes/map/tile.h"

struct RegionWorld {
    Region type;
    bool is_safe;
    std::vector<std::string> npc_types;
    virtual ~RegionWorld() = default;
};

struct WildRegion: RegionWorld {
    uint16_t max_creatures;
    std::optional<uint16_t> min_treasure;
    std::optional<uint16_t> max_treasure;
};

struct SafeRegion: RegionWorld {
    std::vector<uint16_t> numbers_npc;
};

#endif  // REGION_H
