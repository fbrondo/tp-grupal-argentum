#include "client/includes/renderable_entity.h"

#include <cmath>
#include <iostream>

RenderableEntity::RenderableEntity(uint32_t id_, EntityType type_, int start_tile_x_,
                                   int start_tile_y_, uint16_t body_id_, uint16_t head_id_,
                                   uint8_t weapon_id_, uint8_t shield_id_, uint8_t helmet_id_,
                                   uint8_t armor_id_, uint8_t level_, bool is_short_race_):
        id(id_),
        level(level_),
        type(type_),
        is_short_race(is_short_race_),
        tile_x(start_tile_x_),
        tile_y(start_tile_y_),
        current_pixel_x(static_cast<float>(start_tile_x_ * TILE_SIZE)),
        current_pixel_y(static_cast<float>(start_tile_y_ * TILE_SIZE)),
        current_dir(DOWN),
        is_moving(false),
        movement_speed(type_ == EntityType::NPC ? 60.0f : 120.0f),
        body_id(body_id_),
        head_id(head_id_),
        weapon_id(weapon_id_),
        shield_id(shield_id_),
        helmet_id(helmet_id_),
        armor_id(armor_id_),
        is_ghost(false),
        current_hp(0),
        max_hp(0) {
    anim_state.current_anim_id = "";
    anim_state.start_time = SDL_GetTicks();
    anim_state.is_looping = true;
}

// --- Helpers --- //

bool RenderableEntity::is_static_npc_type() const {
    return type == EntityType::NPC || type == EntityType::CITIZEN;
}

std::string RenderableEntity::get_body_prefix() const {
    return is_static_npc_type() ? "npc_" : "body_";
}

std::string RenderableEntity::get_body_anim_id(const std::string& action, Direction dir) const {
    return get_body_prefix() + std::to_string(body_id) + action + std::to_string(dir);
}

int RenderableEntity::calculate_helmet_y_offset() const {
    int helm_y_offset = 0;
    switch (current_dir) {
        case UP:
            helm_y_offset = 0;
            break;
        case DOWN:
            helm_y_offset = is_short_race ? 12 : 14;
            break;
        case LEFT:
            helm_y_offset = -13;
            break;
        case RIGHT:
            helm_y_offset = -26;
            break;
    }
    if (helmet_id == MAGIC_HAT) {
        switch (current_dir) {
            case UP:
                helm_y_offset = -10;
                break;
            case DOWN:
                helm_y_offset = 0;
                break;
            case LEFT:
                helm_y_offset = -23;
                break;
            case RIGHT:
                helm_y_offset = -35;
                break;
        }
    }
    return helm_y_offset;
}

// --- State updates --- //

void RenderableEntity::move_to(int target_tile_x, int target_tile_y, Direction dir) {
    if (type == EntityType::ITEM)
        return;

    current_dir = dir;
    if (tile_x != target_tile_x || tile_y != target_tile_y) {
        const int distance_tiles =
                std::abs(tile_x - target_tile_x) + std::abs(tile_y - target_tile_y);
        tile_x = target_tile_x;
        tile_y = target_tile_y;
        if (distance_tiles > 1) {
            current_pixel_x = static_cast<float>(tile_x * TILE_SIZE);
            current_pixel_y = static_cast<float>(tile_y * TILE_SIZE);
            is_moving = false;
            anim_state.current_anim_id = get_body_anim_id("_idle_", dir);
            anim_state.start_time = SDL_GetTicks();
            return;
        }
        is_moving = true;

        std::string new_anim = get_body_anim_id("_walk_", dir);
        if (anim_state.current_anim_id != new_anim) {
            anim_state.current_anim_id = new_anim;
            anim_state.start_time = SDL_GetTicks();
        }
    } else {
        const float target_pixel_x = static_cast<float>(tile_x * TILE_SIZE);
        const float target_pixel_y = static_cast<float>(tile_y * TILE_SIZE);
        if (current_pixel_x != target_pixel_x || current_pixel_y != target_pixel_y) {
            is_moving = true;
            return;
        }

        is_moving = false;
        anim_state.current_anim_id = get_body_anim_id("_idle_", dir);
    }
}

void RenderableEntity::update(float dt) {
    if (type == EntityType::ITEM)
        return;

    float target_pixel_x = static_cast<float>(tile_x * TILE_SIZE);
    float target_pixel_y = static_cast<float>(tile_y * TILE_SIZE);

    if (current_pixel_x != target_pixel_x || current_pixel_y != target_pixel_y) {
        float diff_x = target_pixel_x - current_pixel_x;
        float diff_y = target_pixel_y - current_pixel_y;
        float step = movement_speed * dt;

        if (std::abs(diff_x) <= step)
            current_pixel_x = target_pixel_x;
        else
            current_pixel_x += (diff_x > 0 ? step : -step);

        if (std::abs(diff_y) <= step)
            current_pixel_y = target_pixel_y;
        else
            current_pixel_y += (diff_y > 0 ? step : -step);
    } else {
        if (is_moving) {
            is_moving = false;
            anim_state.current_anim_id = get_body_anim_id("_idle_", current_dir);
        }
    }
}

void RenderableEntity::set_equipment(uint8_t weapon_id_, uint8_t shield_id_, uint8_t helmet_id_,
                                     uint8_t armor_id_) {
    this->weapon_id = weapon_id_;
    this->shield_id = shield_id_;
    this->helmet_id = helmet_id_;
    this->armor_id = armor_id_;
}

// --- Chat bubble --- //

void RenderableEntity::set_chat_bubble(const std::string& text) {
    chat_bubble_text = text;
    chat_bubble_start_ticks = SDL_GetTicks();
}

bool RenderableEntity::has_active_chat_bubble() const {
    if (chat_bubble_text.empty())
        return false;
    const uint32_t elapsed = SDL_GetTicks() - chat_bubble_start_ticks;
    return elapsed < 5000;
}

// --- Render helpers --- //

void RenderableEntity::render_item(SDL2pp::Renderer& renderer, TextureManager& texture_manager,
                                   int cam_x, int cam_y, int offset_x, int offset_y) {
    try {
        std::string item_key = "item_" + std::to_string(body_id);
        SDL2pp::Texture& tex = texture_manager.get_texture(item_key);
        int tex_w_ = tex.GetWidth();
        int tex_h_ = tex.GetHeight();
        SDL_Rect dst_rect = {
                static_cast<int>(current_pixel_x) - cam_x + offset_x,
                static_cast<int>(current_pixel_y) + TILE_SIZE - cam_y + offset_y - tex_h_,
                tex_w_,
                tex_h_,
        };
        renderer.Copy(tex, SDL2pp::NullOpt, SDL2pp::Rect(dst_rect));
    } catch (...) {}
}

void RenderableEntity::render_simple_layer(SDL2pp::Renderer& renderer,
                                           TextureManager& texture_manager,
                                           const std::string& prefix, uint16_t item_id,
                                           const std::string& action, uint32_t frame_index,
                                           const SDL_Rect& dst_rect, uint8_t alpha) {
    std::string anim = prefix + std::to_string(item_id) + action + std::to_string(current_dir);
    const AnimationClip& clip = texture_manager.get_animation(anim);
    SDL2pp::Texture& tex = texture_manager.get_texture(prefix + std::to_string(item_id));
    const uint32_t frame = frame_index % clip.frames.size();
    tex.SetAlphaMod(alpha);
    renderer.Copy(tex, SDL2pp::Rect(clip.frames[frame]), SDL2pp::Rect(dst_rect));
    tex.SetAlphaMod(255);
}

void RenderableEntity::render_equipment_layers(SDL2pp::Renderer& renderer,
                                               TextureManager& texture_manager,
                                               const std::string& race_prefix,
                                               const std::string& action, uint32_t frame_index,
                                               const SDL_Rect& dst_rect, uint8_t alpha) {
    if (armor_id != 0) {
        render_simple_layer(renderer, texture_manager, race_prefix, armor_id, action, frame_index,
                            dst_rect, alpha);
    }

    if (weapon_id != 0) {
        render_simple_layer(renderer, texture_manager, race_prefix, weapon_id, action, frame_index,
                            dst_rect, alpha);
    }

    SDL_Rect dst_head = {0, 0, 0, 0};
    const bool has_head = head_id != 0;
    if (has_head) {
        std::string head_tex_key = "head_" + std::to_string(head_id);
        std::string head_anim = head_tex_key + "_idle_" + std::to_string(current_dir);
        const AnimationClip& head_clip = texture_manager.get_animation(head_anim);
        SDL_Rect head_src = head_clip.frames[0];
        SDL2pp::Texture& head_texture = texture_manager.get_texture(head_tex_key);
        dst_head = {dst_rect.x + (dst_rect.w - head_src.w) / 2, dst_rect.y, head_src.w, head_src.h};
        if (is_short_race) {
            dst_head.y -= 11;
        } else {
            dst_head.y -= 20;
        }
        head_texture.SetAlphaMod(alpha);
        renderer.Copy(head_texture, SDL2pp::Rect(head_src), SDL2pp::Rect(dst_head));
        head_texture.SetAlphaMod(255);
    }

    if (helmet_id != 0) {
        std::string helm_anim =
                race_prefix + std::to_string(helmet_id) + action + std::to_string(current_dir);
        const AnimationClip& helm_clip = texture_manager.get_animation(helm_anim);
        SDL2pp::Texture& helm_tex =
                texture_manager.get_texture(race_prefix + std::to_string(helmet_id));
        const uint32_t helm_frame = frame_index % helm_clip.frames.size();
        SDL_Rect helm_src = helm_clip.frames[helm_frame];

        int helm_y_offset = calculate_helmet_y_offset();
        int base_x = has_head ? dst_head.x + (dst_head.w - helm_src.w) / 2 :
                                dst_rect.x + (dst_rect.w - helm_src.w) / 2;
        int base_y = (has_head ? dst_head.y : dst_rect.y) + helm_y_offset;
        SDL_Rect dst_helm = {base_x, base_y, helm_src.w, helm_src.h};
        helm_tex.SetAlphaMod(alpha);
        renderer.Copy(helm_tex, SDL2pp::Rect(helm_src), SDL2pp::Rect(dst_helm));
        helm_tex.SetAlphaMod(255);
    }

    if (shield_id != 0) {
        render_simple_layer(renderer, texture_manager, race_prefix, shield_id, action, frame_index,
                            dst_rect, alpha);
    }
}

void RenderableEntity::render_npc_or_player(SDL2pp::Renderer& renderer,
                                            TextureManager& texture_manager, int cam_x, int cam_y,
                                            int offset_x, int offset_y) {
    std::string prefix = get_body_prefix();
    std::string action = is_moving ? "_walk_" : "_idle_";
    std::string anim_id = get_body_anim_id(action, current_dir);

    const AnimationClip* body_clip = nullptr;
    try {
        body_clip = &texture_manager.get_animation(anim_id);
    } catch (const std::exception&) {
        if (!is_static_npc_type()) {
            throw;
        }
        SDL2pp::Texture& npc_texture =
                texture_manager.get_texture(prefix + std::to_string(body_id));
        SDL_Rect dst_rect = {static_cast<int>(current_pixel_x) - cam_x + offset_x,
                             static_cast<int>(current_pixel_y) + TILE_SIZE - cam_y + offset_y -
                                     npc_texture.GetHeight(),
                             npc_texture.GetWidth(), npc_texture.GetHeight()};
        renderer.Copy(npc_texture, SDL2pp::NullOpt, SDL2pp::Rect(dst_rect));
        return;
    }
    uint32_t frame_index = texture_manager.get_current_animation_frame(anim_state, *body_clip);
    SDL_Rect src_rect = body_clip->frames[frame_index];

    SDL_Rect dst_rect = {
            static_cast<int>(current_pixel_x) - cam_x + offset_x,
            static_cast<int>(current_pixel_y) + TILE_SIZE - cam_y + offset_y - src_rect.h,
            src_rect.w, src_rect.h};
    SDL2pp::Texture& body_texture = texture_manager.get_texture(prefix + std::to_string(body_id));
    const uint8_t alpha = is_ghost ? 120 : 255;
    body_texture.SetAlphaMod(alpha);
    renderer.Copy(body_texture, SDL2pp::Rect(src_rect), SDL2pp::Rect(dst_rect));
    body_texture.SetAlphaMod(255);

    if (is_static_npc_type())
        return;

    std::string race_prefix = is_short_race ? "anim_drf_gnm_" : "anim_hum_elf_";
    render_equipment_layers(renderer, texture_manager, race_prefix, action, frame_index, dst_rect,
                            alpha);
}

// --- Rendering --- //

void RenderableEntity::render_with_camera(SDL2pp::Renderer& renderer,
                                          TextureManager& texture_manager, int cam_x, int cam_y,
                                          int offset_x, int offset_y) {
    if (type == EntityType::ITEM) {
        render_item(renderer, texture_manager, cam_x, cam_y, offset_x, offset_y);
        return;
    }

    try {
        render_npc_or_player(renderer, texture_manager, cam_x, cam_y, offset_x, offset_y);
    } catch (const std::exception& e) {
        std::cerr << "[RenderableEntity] " << e.what() << std::endl;
    }
}
