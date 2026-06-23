#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "common/includes/types.h"

struct VisualEffectClip {
    std::vector<std::string> frame_texture_ids;
    uint32_t frame_rate_ms;
};

struct ActiveVisualEffect {
    VisualEffectID effect_id;
    uint32_t pos_x;
    uint32_t pos_y;
    uint32_t start_time;
};
