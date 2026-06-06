#include "client/includes/texture_manager.h"

#include <iostream>
#include <memory>

#include <SDL2/SDL_image.h>
#include <SDL2pp/SDL2pp.hh>

#include "common/includes/direction.h"

TextureManager::TextureManager(SDL2pp::Renderer& renderer_, WindowSDL& window_):
        renderer(renderer_), window(window_) {
    try {
        const SDL2pp::Surface icon("client/assets/icon.png");
        window.setIcon(icon);
    } catch (const std::exception& e) {
        std::cerr << "No se pudo cargar el icono: " << e.what() << std::endl;
    }
}

void TextureManager::register_spritesheet(const std::string& base_id, int frame_width,
                                                   int frame_height, int frames_per_row,
                                                   uint32_t speed_ms) {
    // Fila 0 (y = 0 * h) -> Sur (DOWN)
    // Fila 1 (y = 1 * h) -> Norte (UP)
    // Fila 2 (y = 2 * h) -> Oeste (LEFT)
    // Fila 3 (y = 3 * h) -> Este (RIGHT)

    // Registro las animaciones de caminata
    register_grid_animation(base_id + "_walk_" + std::to_string(DOWN), 0, 0 * frame_height,
                            frame_width, frame_height, frames_per_row, speed_ms);
    register_grid_animation(base_id + "_walk_" + std::to_string(UP), 0, 1 * frame_height,
                            frame_width, frame_height, frames_per_row, speed_ms);
    register_grid_animation(base_id + "_walk_" + std::to_string(LEFT), 0, 2 * frame_height,
                            frame_width, frame_height, frames_per_row, speed_ms);
    register_grid_animation(base_id + "_walk_" + std::to_string(RIGHT), 0, 3 * frame_height,
                            frame_width, frame_height, frames_per_row, speed_ms);

    // Registro las animaciones "idle" (usando el primer cuadro de cada fila)
    register_grid_animation(base_id + "_idle_" + std::to_string(DOWN), 0, 0 * frame_height,
                            frame_width, frame_height, 1, speed_ms);
    register_grid_animation(base_id + "_idle_" + std::to_string(UP), 0, 1 * frame_height,
                            frame_width, frame_height, 1, speed_ms);
    register_grid_animation(base_id + "_idle_" + std::to_string(LEFT), 0, 2 * frame_height,
                            frame_width, frame_height, 1, speed_ms);
    register_grid_animation(base_id + "_idle_" + std::to_string(RIGHT), 0, 3 * frame_height,
                            frame_width, frame_height, 1, speed_ms);
}

void TextureManager::load_all_game_assets() {
    std::cout << "[TextureManager] Cargando assets..." << std::endl;

    // --- EJEMPLO DE CARGA DE CUERPO HUMANO (ID 1036) ---
    // Dimensiones de ejemplo (ajustar según tus PNGs)
    load_texture("citizen_leather", "common/assets/units/bodies/humanos/1036.png");
    register_spritesheet("1036", 32, 45, 4, 150);

    // --- EJEMPLO DE CARGA DE CRIATURA (ID 300) ---
    load_texture("field_goblin", "client/assets/Sprites/Units/criatures/300.png");
    register_spritesheet("body_300", 32, 32, 4, 200);

    // --- EJEMPLO DE CABEZA (HUMANO 3060) ---
    load_texture("brown_spiky_beard", "common/assets/units/heads/humanos/3060.png");

    std::cout << "[TextureManager] Carga finalizada." << std::endl;
}

bool TextureManager::load_texture(const std::string& id, const std::string& filename) {
    if (textures.contains(id)) {
        return true;
    }
    try {
        SDL2pp::Surface surface(filename);
        surface.SetColorKey(true, 0);
        textures[id] = std::make_unique<SDL2pp::Texture>(renderer, surface);
        return true;

    } catch (const std::exception& e) {
        std::cerr << "Error al cargar la textura [" << id << "] desde " << filename << ": "
                  << e.what() << std::endl;
        return false;
    }
}

void TextureManager::register_grid_animation(const std::string& anim_id,
                                             /*const std::string& texture_id,*/ int start_x,
                                             int start_y, int frame_width, int frame_height,
                                             int frame_count, uint32_t speed_ms) {

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

const SDL2pp::Texture& TextureManager::get_texture(const std::string& id) const {
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
    // 1. Tiempo transcurrido (siempre positivo)
    uint32_t elapsed_time = SDL_GetTicks() - state.start_time;
    // 2. Cuadros transcurridos
    uint32_t total_frames_elapsed = elapsed_time / clip.frame_rate_ms;
    // 3. Cantidad de cuadros (clip.frames.size() devuelve un size_t, lo casteamos a uint32_t)
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
