#include "client/includes/texture_manager.h"

#include <filesystem>
#include <iostream>
#include <memory>

#include <SDL2/SDL_image.h>
#include <SDL2pp/SDL2pp.hh>

#include "client/includes/asset_catalog_loader.h"
#include "client/includes/core/texture.h"
#include "common/includes/direction.h"
#include "common/includes/types.h"

TextureManager::TextureManager(SDL2pp::Renderer& renderer_, WindowSDL& window_):
        renderer(renderer_), window(window_) {
    try {
        const SDL2pp::Surface icon(ARGENTUM_SHARE_PATH "/client/assets/icon.png");
        window.setIcon(icon);
    } catch (const std::exception& e) {
        std::cerr << "No se pudo cargar el icono: " << e.what() << std::endl;
    }
    this->textures = load_all_game_assets();
}

void TextureManager::register_spritesheet(const std::string& base_id, int frame_width,
                                          int frame_height, uint8_t frames_per_row,
                                          uint32_t speed_ms) {
    constexpr std::array<uint8_t, 4> dirs = {DOWN, UP, LEFT, RIGHT};
    for (size_t i = 0; i < dirs.size(); ++i) {
        register_grid_animation(base_id + "_walk_" + std::to_string(dirs[i]), 0, i * frame_height,
                                frame_width, frame_height, frames_per_row, speed_ms);
        register_grid_animation(base_id + "_idle_" + std::to_string(dirs[i]), 0, i * frame_height,
                                frame_width, frame_height, 1, speed_ms);
    }
}

void TextureManager::load_heads_textures(
        std::unordered_map<std::string, std::unique_ptr<SDL2pp::Texture>>& textures_aux) {
    auto entries = load_asset_catalog(ARGENTUM_SHARE_PATH "/client/assets/config/heads.toml");
    for (const auto& e: entries) {
        const std::string tex_id = "head_" + std::to_string(e.id);
        load_texture(textures_aux, tex_id, e.filepath);
        register_spritesheet(tex_id, e.frame_width, e.frame_height,
                             static_cast<uint8_t>(e.frames_per_row));
    }
}

void TextureManager::load_bodies_textures(
        std::unordered_map<std::string, std::unique_ptr<SDL2pp::Texture>>& textures_aux) {
    auto entries = load_asset_catalog(ARGENTUM_SHARE_PATH "/client/assets/config/bodies.toml");
    for (const auto& e: entries) {
        const std::string tex_id = "body_" + std::to_string(e.id);
        load_texture(textures_aux, tex_id, e.filepath);
        register_spritesheet(tex_id, e.frame_width, e.frame_height,
                             static_cast<uint8_t>(e.frames_per_row));
    }
}

void TextureManager::load_npcs_textures(
        std::unordered_map<std::string, std::unique_ptr<SDL2pp::Texture>>& textures_aux) {
    auto entries = load_asset_catalog(ARGENTUM_SHARE_PATH "/client/assets/config/npcs.toml");
    for (const auto& e: entries) {
        const std::string tex_id = "npc_" + std::to_string(e.id);
        load_texture(textures_aux, tex_id, e.filepath);
        if (e.frames_per_row > 0) {
            register_spritesheet(tex_id, e.frame_width, e.frame_height,
                                 static_cast<uint8_t>(e.frames_per_row));
        }
    }
}

void TextureManager::load_items_textures(
        std::unordered_map<std::string, std::unique_ptr<SDL2pp::Texture>>& textures_aux) {
    auto entries = load_asset_catalog(ARGENTUM_SHARE_PATH "/client/assets/config/items.toml");
    for (const auto& e: entries) {
        const std::string tex_id = "item_" + std::to_string(e.id);
        load_texture(textures_aux, tex_id, e.filepath);
    }
}

void TextureManager::load_animation_items(
        std::unordered_map<std::string, std::unique_ptr<SDL2pp::Texture>>& textures_aux) {
    auto entries =
            load_asset_catalog(ARGENTUM_SHARE_PATH "/client/assets/config/animated_items.toml");
    for (const auto& e: entries) {
        const std::string tex_id = e.prefix + std::to_string(e.id);
        load_texture(textures_aux, tex_id, e.filepath);
        register_spritesheet(tex_id, e.frame_width, e.frame_height,
                             static_cast<uint8_t>(e.frames_per_row));
    }
}

void TextureManager::load_tile_textures(
        std::unordered_map<std::string, std::unique_ptr<SDL2pp::Texture>>& textures_aux) {
    const std::array<std::pair<std::string, std::string>, 4> layers = {{
            {ARGENTUM_SHARE_PATH "/common/assets/maps/background/", "tile_bg_"},
            {ARGENTUM_SHARE_PATH "/common/assets/maps/details/", "tile_det_"},
            {ARGENTUM_SHARE_PATH "/common/assets/maps/objets/", "tile_obj_"},
            {ARGENTUM_SHARE_PATH "/common/assets/maps/roof/", "tile_roof_"},
    }};
    for (const auto& [dir, prefix]: layers) {
        if (!std::filesystem::exists(dir)) {
            std::cout << "[TextureManager] DIRECTORIO NO ENCONTRADO: " << dir << std::endl;
            continue;
        }
        for (const auto& entry: std::filesystem::directory_iterator(dir)) {
            if (entry.path().extension() != ".png")
                continue;
            const std::string id = entry.path().stem().string();
            load_texture(textures_aux, prefix + id, entry.path().string());
        }
    }
}

void TextureManager::load_HUD_textures(
        std::unordered_map<std::string, std::unique_ptr<SDL2pp::Texture>>& textures_aux) {
    load_texture(textures_aux, "hud_upper_layer", "client/assets/HUD/UpperLayer.png");
    load_texture(textures_aux, "hud_console_base", "client/assets/HUD/Console/base.png");
    load_texture(textures_aux, "hud_stats_base", "client/assets/HUD/UserStats/base.png");
    load_texture(textures_aux, "hud_user_info_base", "client/assets/HUD/UserInfo/base.png");
    load_texture(textures_aux, "hud_inventory_base", "client/assets/HUD/UserInventory/base.png");
    load_texture(textures_aux, "hud_inventory_selected",
                 "client/assets/HUD/UserInventory/selected.png");
    load_texture(textures_aux, "hud_pb_base", "client/assets/HUD/ProgressBar/base.png");
    load_texture(textures_aux, "hud_pb_bisel", "client/assets/HUD/ProgressBar/bisel.png");
    load_texture(textures_aux, "hud_pb_black_border",
                 "client/assets/HUD/ProgressBar/black_border.png");
    load_texture(textures_aux, "hud_pb_white_border",
                 "client/assets/HUD/ProgressBar/white_border.png");
    load_texture(textures_aux, "hud_pb_old_base", "client/assets/HUD/ProgressBar/old_base.png");
}

std::unordered_map<std::string, std::unique_ptr<SDL2pp::Texture>>
        TextureManager::load_all_game_assets() {
    std::unordered_map<std::string, std::unique_ptr<SDL2pp::Texture>> textures_aux;
    load_items_textures(textures_aux);
    load_animation_items(textures_aux);
    load_heads_textures(textures_aux);
    load_bodies_textures(textures_aux);
    load_npcs_textures(textures_aux);
    load_tile_textures(textures_aux);
    load_HUD_textures(textures_aux);
    return textures_aux;
}

bool TextureManager::load_texture(
        std::unordered_map<std::string, std::unique_ptr<SDL2pp::Texture>>& textures_,
        const std::string& id, const std::string& filename) {
    if (textures_.contains(id)) {
        return true;
    }
    try {
        SDL2pp::Surface surface(filename);
        surface.SetColorKey(true, 0);
        textures_[id] = std::make_unique<SDL2pp::Texture>(renderer, surface);
        return true;

    } catch (const std::exception& e) {
        std::cerr << "Error al cargar la textura [" << id << "] desde " << filename << ": "
                  << e.what() << std::endl;
        return false;
    }
}

void TextureManager::register_grid_animation(const std::string& anim_id, int start_x, int start_y,
                                             int frame_width, int frame_height, int frame_count,
                                             uint32_t speed_ms) {
    AnimationClip clip;
    clip.frame_rate_ms = speed_ms;

    for (int i = 0; i < frame_count; ++i) {
        SDL_Rect rect;
        rect.x = start_x + (i * frame_width);
        rect.y = start_y;
        rect.w = frame_width;
        rect.h = frame_height;

        clip.frames.push_back(rect);
    }
    animations[anim_id] = clip;
}

SDL2pp::Texture& TextureManager::get_texture(const std::string& id) const {
    const auto it = textures.find(id);
    if (it == textures.end()) {
        throw std::runtime_error("Textura no encontrada: " + id);
    }
    return *(it->second);
}

const AnimationClip& TextureManager::get_animation(const std::string& anim_id) const {
    const auto it = animations.find(anim_id);
    if (it == animations.end()) {
        throw std::runtime_error("Animacion no encontrada: " + anim_id);
    }
    return it->second;
}

uint32_t TextureManager::get_current_animation_frame(const AnimationState& state,
                                                     const AnimationClip& clip) {
    uint32_t elapsed_time = SDL_GetTicks() - state.start_time;
    uint32_t total_frames_elapsed = elapsed_time / clip.frame_rate_ms;
    uint32_t num_frames = static_cast<uint32_t>(clip.frames.size());
    if (num_frames == 0)
        return 0;
    if (state.is_looping) {
        return total_frames_elapsed % num_frames;
    }
    if (total_frames_elapsed >= num_frames) {
        return num_frames - 1;
    }
    return total_frames_elapsed;
}
