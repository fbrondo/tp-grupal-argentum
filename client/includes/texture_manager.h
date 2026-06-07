#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <SDL2/SDL.h>
#include <SDL2pp/SDL2pp.hh>

#include "client/includes/window/windowSDL.h"

static constexpr uint16_t DEFAULT_ANIMATION_SPEED_MS = 150;

struct AnimationClip {
    std::vector<SDL_Rect> frames;
    uint32_t frame_rate_ms;
};

struct AnimationState {
    std::string current_anim_id;
    uint32_t start_time;
    bool is_looping;
};

class TextureManager {
private:
    SDL2pp::Renderer& renderer;
    WindowSDL& window;

    std::unordered_map<std::string, std::unique_ptr<SDL2pp::Texture>> textures;
    std::unordered_map<std::string, AnimationClip> animations;

    bool load_texture(std::unordered_map<std::string, std::unique_ptr<SDL2pp::Texture>>& textures_,
                      const std::string& id, const std::string& filename);

    void register_grid_animation(const std::string& anim_id, int start_x, int start_y,
                                 int frame_width, int frame_height, int frame_count,
                                 uint32_t speed_ms);

    void register_spritesheet(const std::string& base_id, int frame_width, int frame_height,
                              uint8_t frames_per_row,
                              uint32_t speed_ms = DEFAULT_ANIMATION_SPEED_MS);

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

public:
    explicit TextureManager(SDL2pp::Renderer& renderer_, WindowSDL& window_);
    ~TextureManager() = default;

    TextureManager(const TextureManager&) = delete;
    TextureManager& operator=(const TextureManager&) = delete;

    std::unordered_map<std::string, std::unique_ptr<SDL2pp::Texture>> load_all_game_assets();

    SDL2pp::Texture& get_texture(const std::string& id) const;
    const AnimationClip& get_animation(const std::string& anim_id) const;
    uint32_t get_current_animation_frame(const AnimationState& state, const AnimationClip& clip);
};
