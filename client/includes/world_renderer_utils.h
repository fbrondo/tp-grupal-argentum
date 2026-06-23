#pragma once

#include <algorithm>
#include <array>
#include <cstdint>
#include <sstream>
#include <string>
#include <unordered_set>
#include <vector>

#include <SDL2pp/SDL2pp.hh>

#include "client/includes/core/animation.h"
#include "client/includes/core/constants.h"
#include "client/includes/renderable_entity.h"
#include "client/includes/texture_manager.h"
#include "common/includes/core/snapshot.h"
#include "common/includes/types.h"

namespace world_renderer_utils {

inline uint32_t player_entity_key(uint32_t server_id) { return PLAYER_ENTITY_OFFSET + server_id; }

inline uint32_t npc_entity_key(uint32_t server_id) { return NPC_ENTITY_OFFSET + server_id; }

inline uint32_t citizen_entity_key(uint32_t server_id) { return CITIZEN_ENTITY_OFFSET + server_id; }

inline uint32_t item_entity_key(uint32_t pos_x, uint32_t pos_y) {
    return ITEM_ENTITY_OFFSET + (pos_x * 1000) + pos_y;
}

inline uint32_t exp_next_level(uint8_t level) {
    return static_cast<uint32_t>(1000 * std::pow(level, 1.8));
}

inline int64_t squared_distance(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2) {
    const int64_t dx = static_cast<int64_t>(x1) - static_cast<int64_t>(x2);
    const int64_t dy = static_cast<int64_t>(y1) - static_cast<int64_t>(y2);
    return dx * dx + dy * dy;
}

inline bool is_entity_inside_world_rect(const RenderableEntity& entity, const SDL_Rect& rect) {
    const SDL_Point entity_center = {
            static_cast<int>(entity.get_pixel_x()) + TILE_SIZE / 2,
            static_cast<int>(entity.get_pixel_y()) + TILE_SIZE / 2,
    };
    return SDL_PointInRect(&entity_center, &rect);
}

int sound_effect_priority(SoundEffectID effect_id);

bool is_same_sound_nearby(const SoundEffectSnapshotData& first,
                          const SoundEffectSnapshotData& second, int64_t max_distance_sq);

bool was_similar_sound_already_selected(const std::vector<SoundEffectSnapshotData>& selected,
                                        const SoundEffectSnapshotData& candidate,
                                        int64_t max_distance_sq);

std::vector<SoundEffectSnapshotData> select_audible_sound_effects(
        const std::vector<SoundEffectSnapshotData>& effects, uint32_t player_x, uint32_t player_y);

void log_render_error_once(const std::string& texture_key, const std::exception& e);

std::vector<std::string> wrap_text(SDL2pp::Font& font, const std::string& text, int max_width);

SDL_Rect calculate_visual_effect_dst(SDL2pp::Texture& texture, VisualEffectID effect_id,
                                     uint32_t pos_x, uint32_t pos_y, const SDL_Rect& camera,
                                     int offset_x, int offset_y);

}  // namespace world_renderer_utils
