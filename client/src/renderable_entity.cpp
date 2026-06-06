#include "client/includes/renderable_entity.h"

#include <cmath>
#include <iostream>

static constexpr int TILE_SIZE = 32;

RenderableEntity::RenderableEntity(uint32_t id, int start_tile_x, int start_tile_y,
                                   uint8_t body_id_, uint8_t head_id_, uint8_t weapon_id_,
                                   uint8_t shield_id_):
        id(id),
        tile_x(start_tile_x),
        tile_y(start_tile_y),
        current_pixel_x(static_cast<float>(start_tile_x * TILE_SIZE)),
        current_pixel_y(static_cast<float>(start_tile_y * TILE_SIZE)),
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

// El método move_to se ejecuta cuando llega un snapshot con una posición nueva
void RenderableEntity::move_to(int target_tile_x, int target_tile_y, Direction dir) {
    current_dir = dir;

    // Si las baldosas cambiaron, significa que nos estamos moviendo
    if (tile_x != target_tile_x || tile_y != target_tile_y) {
        tile_x = target_tile_x;
        tile_y = target_tile_y;
        is_moving = true;

        // Si no estábamos en una animación de movimiento, la seteamos
        // Ejemplo de ID de animación: "body_1036_walk_up
        std::string new_anim = "body_" + std::to_string(body_id) + "_walk_" + std::to_string(dir);
        if (anim_state.current_anim_id != new_anim) {
            anim_state.current_anim_id = new_anim;
            anim_state.start_time = SDL_GetTicks();  // Reiniciamos el reloj de la animación
        }
    } else {
        // Si el servidor nos mandó la misma baldosa pero otra dirección, solo giramos quietos
        // (idle)
        is_moving = false;
        anim_state.current_anim_id =
                "body_" + std::to_string(body_id) + "_idle_" + std::to_string(dir);
    }
}

void RenderableEntity::update(float dt) {
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
        if (std::abs(diff_x) <= step) {
            current_pixel_x = target_pixel_x;  // Si estamos muy cerca, nos pegamos al destino
        } else {
            current_pixel_x += (diff_x > 0 ? step : -step);
        }

        // Interpolación en Y
        if (std::abs(diff_y) <= step) {
            current_pixel_y = target_pixel_y;
        } else {
            current_pixel_y += (diff_y > 0 ? step : -step);
        }
    } else {
        // Si ya alcanzamos los píxeles de la baldosa, dejamos de movernos
        if (is_moving) {
            is_moving = false;
            // Volvemos a animación quieto (idle)
            anim_state.current_anim_id =
                    "body_" + std::to_string(body_id) + "_idle_" + std::to_string(current_dir);
        }
    }
}

void RenderableEntity::render_with_camera(SDL2pp::Renderer& renderer,
                                          TextureManager& texture_manager, int cam_x, int cam_y,
                                          int offset_x, int offset_y) {

    // 1. Determinar el ID de la animación según el estado
    std::string action = is_moving ? "_walk_" : "_idle_";
    std::string anim_id = "body_" + std::to_string(body_id) + action + std::to_string(current_dir);

    // 2. Obtener el clip y el frame actual desde el TextureManager
    try {
        const AnimationClip& clip = texture_manager.get_animation(anim_id);
        uint32_t frame_index = texture_manager.get_current_animation_frame(anim_state, clip);
        SDL_Rect src_rect = clip.frames[frame_index];

        // 3. Calcular posición final (World -> Camera -> Screen)
        SDL_Rect dst_rect;
        dst_rect.x = static_cast<int>(current_pixel_x) - cam_x + offset_x;
        dst_rect.y = static_cast<int>(current_pixel_y) - cam_y + offset_y;
        dst_rect.w = src_rect.w;
        dst_rect.h = src_rect.h;

        // 4. Dibujar cuerpo
        std::string body_tex_key = "body_" + std::to_string(body_id);
        SDL2pp::Texture& body_texture = texture_manager.get_texture(body_tex_key);
        renderer.Copy(body_texture, SDL2pp::Rect(src_rect), SDL2pp::Rect(dst_rect));

        // 5. Dibujar cabeza (si tiene una asignada)
        if (head_id != 0) {
            std::string head_tex_key = "head_" + std::to_string(head_id);
            SDL2pp::Texture& head_texture = texture_manager.get_texture(head_tex_key);

            // La cabeza suele ir centrada sobre el cuerpo y un poquito más arriba
            SDL_Rect dst_head = dst_rect;
            dst_head.y -= 10;  // Offset vertical para la cabeza
            renderer.Copy(head_texture, SDL2pp::NullOpt, SDL2pp::Rect(dst_head));
        }

    } catch (const std::exception& e) {
        // Manejo de error silencioso: si falta una textura, la entidad no se dibuja (evita crasheo)
    }
}
