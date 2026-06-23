#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <SDL2pp/SDL2pp.hh>

#include "client/includes/core/animation.h"
#include "client/includes/core/constants.h"
#include "client/includes/core/visual_effect.h"
#include "client/includes/window/windowSDL.h"
#include "common/includes/types.h"

class TextureManager {
private:
    SDL2pp::Renderer& renderer;

    std::unordered_map<std::string, std::unique_ptr<SDL2pp::Texture>> textures;
    std::unordered_map<std::string, AnimationClip> animations;
    std::unordered_map<VisualEffectID, VisualEffectClip> visual_effects;

    bool load_texture(std::unordered_map<std::string, std::unique_ptr<SDL2pp::Texture>>& textures_,
                      const std::string& id, const std::string& filename);

    void register_grid_animation(const std::string& anim_id, int start_x, int start_y,
                                 int frame_width, int frame_height, int frame_count,
                                 uint32_t speed_ms);

    void register_spritesheet(const std::string& base_id, int frame_width, int frame_height,
                              uint8_t frames_per_row,
                              uint32_t speed_ms = DEFAULT_ANIMATION_SPEED_MS);

    std::unordered_map<std::string, std::unique_ptr<SDL2pp::Texture>> load_all_game_assets();

    void load_heads_textures(
            std::unordered_map<std::string, std::unique_ptr<SDL2pp::Texture>>& textures_aux);
    void load_bodies_textures(
            std::unordered_map<std::string, std::unique_ptr<SDL2pp::Texture>>& textures_aux);
    void load_npcs_textures(
            std::unordered_map<std::string, std::unique_ptr<SDL2pp::Texture>>& textures_aux);
    void load_items_textures(
            std::unordered_map<std::string, std::unique_ptr<SDL2pp::Texture>>& textures_aux);
    void load_animation_items(
            std::unordered_map<std::string, std::unique_ptr<SDL2pp::Texture>>& textures_aux);
    void load_tile_textures(
            std::unordered_map<std::string, std::unique_ptr<SDL2pp::Texture>>& textures_aux);
    void load_effect_textures(
            std::unordered_map<std::string, std::unique_ptr<SDL2pp::Texture>>& textures_aux);
    void load_HUD_textures(
            std::unordered_map<std::string, std::unique_ptr<SDL2pp::Texture>>& textures_aux);

public:
    explicit TextureManager(SDL2pp::Renderer& renderer_, WindowSDL& window_);
    ~TextureManager() = default;

    TextureManager(const TextureManager&) = delete;
    TextureManager& operator=(const TextureManager&) = delete;

    SDL2pp::Texture& get_texture(const std::string& id) const;
    const AnimationClip& get_animation(const std::string& anim_id) const;
    const VisualEffectClip& get_visual_effect(VisualEffectID effect_id) const;
    static uint32_t get_current_animation_frame(const AnimationState& state,
                                                const AnimationClip& clip);
};
