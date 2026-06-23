#include "client/includes/world_renderer_utils.h"

#include <cmath>
#include <iostream>

#include "common/includes/core/snapshot.h"

namespace world_renderer_utils {

static constexpr std::array<std::pair<SoundEffectID, int>, 23> SOUND_PRIORITIES = {{
        {SoundEffectID::MUERTE_HOMBRE, 4},
        {SoundEffectID::RESUCITAR, 4},
        {SoundEffectID::RESUCITAR_SACERDOTE, 4},
        {SoundEffectID::GOLPE_RECIBIDO, 3},
        {SoundEffectID::GOLPE_ARMA, 3},
        {SoundEffectID::ESPADAZO, 3},
        {SoundEffectID::FLECHA, 3},
        {SoundEffectID::FLECHA_MAGICA, 3},
        {SoundEffectID::RESORTE_EXPLOSIVO, 3},
        {SoundEffectID::CURAR, 2},
        {SoundEffectID::CURAR_2, 2},
        {SoundEffectID::TOMAR_POCION, 2},
        {SoundEffectID::EQUIPAR_ARMA, 2},
        {SoundEffectID::DROP_ESPECIAL_NPC, 2},
        {SoundEffectID::PASO, 0},
        {SoundEffectID::PASO_2, 0},
        {SoundEffectID::PASOS, 0},
        {SoundEffectID::PASOS_EN_GRAVA, 0},
        {SoundEffectID::PASO_3, 0},
        {SoundEffectID::PASO_4, 0},
        {SoundEffectID::PASO_5, 0},
        {SoundEffectID::PASO_6, 0},
        {SoundEffectID::PASO_7, 0},
}};

int sound_effect_priority(SoundEffectID effect_id) {
    const auto it =
            std::find_if(SOUND_PRIORITIES.begin(), SOUND_PRIORITIES.end(),
                         [effect_id](const auto& priority) { return priority.first == effect_id; });
    if (it != SOUND_PRIORITIES.end()) {
        return it->second;
    }
    return 1;
}

bool is_same_sound_nearby(const SoundEffectSnapshotData& first,
                          const SoundEffectSnapshotData& second, int64_t max_distance_sq) {
    return first.effect_id == second.effect_id &&
           squared_distance(first.pos_x, first.pos_y, second.pos_x, second.pos_y) <=
                   max_distance_sq;
}

bool was_similar_sound_already_selected(const std::vector<SoundEffectSnapshotData>& selected,
                                        const SoundEffectSnapshotData& candidate,
                                        int64_t max_distance_sq) {
    for (const auto& selected_effect: selected) {
        if (is_same_sound_nearby(selected_effect, candidate, max_distance_sq)) {
            return true;
        }
    }
    return false;
}

std::vector<SoundEffectSnapshotData> select_audible_sound_effects(
        const std::vector<SoundEffectSnapshotData>& effects, uint32_t player_x, uint32_t player_y) {
    constexpr size_t MAX_SOUNDS_PER_SNAPSHOT = 8;
    constexpr int64_t SAME_SOUND_GROUP_RADIUS_TILES = 2;
    constexpr int64_t SAME_SOUND_GROUP_RADIUS_SQ =
            SAME_SOUND_GROUP_RADIUS_TILES * SAME_SOUND_GROUP_RADIUS_TILES;

    struct Candidate {
        SoundEffectSnapshotData effect;
        int priority;
        int64_t distance_sq;
    };

    std::vector<Candidate> candidates;
    candidates.reserve(effects.size());
    for (const auto& effect: effects) {
        candidates.push_back({effect, sound_effect_priority(effect.effect_id),
                              squared_distance(effect.pos_x, effect.pos_y, player_x, player_y)});
    }

    std::sort(candidates.begin(), candidates.end(), [](const Candidate& a, const Candidate& b) {
        if (a.priority != b.priority) {
            return a.priority > b.priority;
        }
        return a.distance_sq < b.distance_sq;
    });

    std::vector<SoundEffectSnapshotData> selected;
    selected.reserve(std::min(MAX_SOUNDS_PER_SNAPSHOT, effects.size()));

    for (const auto& candidate: candidates) {
        if (was_similar_sound_already_selected(selected, candidate.effect,
                                               SAME_SOUND_GROUP_RADIUS_SQ)) {
            continue;
        }

        selected.push_back(candidate.effect);
        if (selected.size() == MAX_SOUNDS_PER_SNAPSHOT) {
            break;
        }
    }

    return selected;
}

void log_render_error_once(const std::string& texture_key, const std::exception& e) {
    static std::unordered_set<std::string> logged_errors;
    if (logged_errors.insert(texture_key).second) {
        std::cerr << "[WorldRenderer] No se pudo renderizar " << texture_key << ": " << e.what()
                  << std::endl;
    }
}

std::vector<std::string> wrap_text(SDL2pp::Font& font, const std::string& text, int max_width) {
    std::vector<std::string> lines;
    if (text.empty())
        return lines;

    std::istringstream stream(text);
    std::string word;
    std::string current_line;

    while (stream >> word) {
        std::string test_line = current_line.empty() ? word : current_line + " " + word;
        SDL2pp::Surface surface = font.RenderUTF8_Blended(test_line, SDL_Color{255, 255, 255, 255});
        if (surface.GetWidth() > max_width && !current_line.empty()) {
            lines.push_back(current_line);
            current_line = word;
        } else {
            current_line = test_line;
        }
    }
    if (!current_line.empty()) {
        lines.push_back(current_line);
    }
    return lines;
}

SDL_Rect calculate_visual_effect_dst(SDL2pp::Texture& texture, VisualEffectID effect_id,
                                     uint32_t pos_x, uint32_t pos_y, const SDL_Rect& camera,
                                     int offset_x, int offset_y) {
    int dst_w = texture.GetWidth();
    int dst_h = texture.GetHeight();

    if (dst_w >= camera.w && dst_h >= camera.h) {
        return SDL_Rect{offset_x, offset_y, camera.w, camera.h};
    }

    SDL_Rect dst;
    dst.w = dst_w;
    dst.h = dst_h;
    dst.x = static_cast<int>(pos_x * TILE_SIZE) + (TILE_SIZE - dst.w) / 2 - camera.x + offset_x;
    if (effect_id == VisualEffectID::EXPLOSION) {
        dst.y = static_cast<int>(pos_y * TILE_SIZE) + (TILE_SIZE - dst.h) / 2 - camera.y + offset_y;
    } else {
        dst.y = static_cast<int>(pos_y * TILE_SIZE) + TILE_SIZE - dst.h - camera.y + offset_y;
    }
    return dst;
}

}  // namespace world_renderer_utils
