#include "client/includes/renderable_entity.h"

#include <cmath>
#include <iostream>

static constexpr int TILE_SIZE = 32;

RenderableEntity::RenderableEntity(uint32_t id_, EntityType type_, int start_tile_x_,
                                   int start_tile_y_, uint16_t body_id_, uint16_t head_id_,
                                   uint8_t weapon_id_, uint8_t shield_id_, bool is_short_race_):
        id(id_),
        type(type_),
        is_short_race(is_short_race_),
        tile_x(start_tile_x_),
        tile_y(start_tile_y_),
        current_pixel_x(static_cast<float>(start_tile_x_ * TILE_SIZE)),
        current_pixel_y(static_cast<float>(start_tile_y_ * TILE_SIZE)),
        current_dir(DOWN),
        is_moving(false),
        movement_speed(120.0f),
        body_id(body_id_),
        head_id(head_id_),
        weapon_id(weapon_id_),
        shield_id(shield_id_) {

    // Inicializamos el estado de animación local
    anim_state.current_anim_id = "";
    anim_state.start_time = SDL_GetTicks();
    anim_state.is_looping = true;
}

void RenderableEntity::move_to(int target_tile_x, int target_tile_y, Direction dir) {
    if (type == EntityType::ITEM)
        return;  // Los ítems en el piso no giran ni caminan

    current_dir = dir;
    if (tile_x != target_tile_x || tile_y != target_tile_y) {
        tile_x = target_tile_x;
        tile_y = target_tile_y;
        is_moving = true;

        std::string prefix = (type == EntityType::NPC) ? "npc_" : "body_";
        std::string new_anim = prefix + std::to_string(body_id) + "_walk_" + std::to_string(dir);

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
        std::string prefix = (type == EntityType::NPC) ? "npc_" : "body_";
        anim_state.current_anim_id =
                prefix + std::to_string(body_id) + "_idle_" + std::to_string(dir);
    }
}

void RenderableEntity::update(float dt) {
    if (type == EntityType::ITEM)
        return;  // Los ítems no se interpolan

    // 1. Calculamos dónde deberíamos estar en píxeles según la baldosa lógica
    float target_pixel_x = static_cast<float>(tile_x * TILE_SIZE);
    float target_pixel_y = static_cast<float>(tile_y * TILE_SIZE);

    // 2. Si la posición física en píxeles está lejos de la baldosa lógica, nos acercamos suavemente
    if (current_pixel_x != target_pixel_x || current_pixel_y != target_pixel_y) {
        // Calculamos la distancia que nos falta recorrer
        float diff_x = target_pixel_x - current_pixel_x;
        float diff_y = target_pixel_y - current_pixel_y;

        // Cuánto nos moveremos en este frame (Velocidad * tiempo transcurrido dt)
        float step = movement_speed * dt;

        // Interpolación en X
        if (std::abs(diff_x) <= step)
            current_pixel_x = target_pixel_x;  // Si estamos muy cerca, nos pegamos al destino
        else
            current_pixel_x += (diff_x > 0 ? step : -step);

        // Interpolación en Y
        if (std::abs(diff_y) <= step)
            current_pixel_y = target_pixel_y;
        else
            current_pixel_y += (diff_y > 0 ? step : -step);
    } else {
        // Si ya alcanzamos los píxeles de la baldosa, dejamos de movernos
        if (is_moving) {
            is_moving = false;
            std::string prefix = (type == EntityType::NPC) ? "npc_" : "body_";
            anim_state.current_anim_id =
                    prefix + std::to_string(body_id) + "_idle_" + std::to_string(current_dir);
        }
    }
}

void RenderableEntity::render_with_camera(SDL2pp::Renderer& renderer,
                                          TextureManager& texture_manager, int cam_x, int cam_y,
                                          int offset_x, int offset_y) {

    // 1. RENDERIZADO DE ÍTEMS ESTÁTICOS
    if (type == EntityType::ITEM) {
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
        return;  // Terminamos, el ítem no tiene cabeza ni arma
    }

    // 2. RENDERIZADO DE JUGADORES Y NPCs
    try {
        std::string prefix = (type == EntityType::NPC) ? "npc_" : "body_";
        std::string action = is_moving ? "_walk_" : "_idle_";
        std::string anim_id =
                prefix + std::to_string(body_id) + action + std::to_string(current_dir);

        const AnimationClip& body_clip = texture_manager.get_animation(anim_id);
        uint32_t frame_index = texture_manager.get_current_animation_frame(anim_state, body_clip);
        SDL_Rect src_rect = body_clip.frames[frame_index];

        SDL_Rect dst_rect = {static_cast<int>(current_pixel_x) - cam_x + offset_x,
                             static_cast<int>(current_pixel_y) - cam_y + offset_y, src_rect.w,
                             src_rect.h};

        // A) Dibujar Cuerpo o Criatura
        SDL2pp::Texture& body_texture =
                texture_manager.get_texture(prefix + std::to_string(body_id));
        renderer.Copy(body_texture, SDL2pp::Rect(src_rect), SDL2pp::Rect(dst_rect));

        // Si es NPC, termina acá (no usan armas ni cascos sueltos)
        if (type == EntityType::NPC)
            return;

        // B) Dibujar Capas Adicionales solo para PLAYER
        std::string race_prefix = is_short_race ? "anim_drf_gnm_" : "anim_hum_elf_";

        // Arma (Va por detrás de la cabeza, sobre el cuerpo)
        if (weapon_id != 0) {
            std::string wpn_anim =
                    race_prefix + std::to_string(weapon_id) + action + std::to_string(current_dir);
            const AnimationClip& wpn_clip = texture_manager.get_animation(wpn_anim);
            SDL2pp::Texture& wpn_tex =
                    texture_manager.get_texture(race_prefix + std::to_string(weapon_id));
            renderer.Copy(wpn_tex, SDL2pp::Rect(wpn_clip.frames[frame_index]),
                          SDL2pp::Rect(dst_rect));
        }

        // Cabeza
        if (head_id != 0) {
            std::string head_tex_key = "head_" + std::to_string(head_id);
            std::string head_anim = head_tex_key + "_idle_" + std::to_string(current_dir);
            const AnimationClip& head_clip = texture_manager.get_animation(head_anim);
            SDL_Rect head_src = head_clip.frames[0];
            SDL2pp::Texture& head_texture = texture_manager.get_texture(head_tex_key);
            SDL_Rect dst_head = {dst_rect.x + (dst_rect.w - head_src.w) / 2, dst_rect.y, head_src.w,
                                 head_src.h};
            dst_head.y -= 10;  // Offset hacia arriba para el cuello
            renderer.Copy(head_texture, SDL2pp::Rect(head_src), SDL2pp::Rect(dst_head));
        }

        // Escudo (Suele ir en la capa más alta para tapar parte del cuerpo)
        if (shield_id != 0) {
            std::string shd_anim =
                    race_prefix + std::to_string(shield_id) + action + std::to_string(current_dir);
            const AnimationClip& shd_clip = texture_manager.get_animation(shd_anim);
            SDL2pp::Texture& shd_tex =
                    texture_manager.get_texture(race_prefix + std::to_string(shield_id));
            renderer.Copy(shd_tex, SDL2pp::Rect(shd_clip.frames[frame_index]),
                          SDL2pp::Rect(dst_rect));
        }

    } catch (const std::exception& e) {
        std::cerr << "[RenderableEntity] " << e.what() << std::endl;
    }
}
