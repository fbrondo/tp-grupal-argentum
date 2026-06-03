#include "client/includes/texture_manager.h"

#include <iostream>

TextureManager::TextureManager(SDL_Renderer* renderer): renderer(renderer) {}

bool TextureManager::load_texture(const std::string& id, const std::string& filename) {
    // Si ya existe, evitamos recargarla
    if (textures.find(id) != textures.end()) {
        return true;
    }

    SDL_Texture* texture = IMG_LoadTexture(renderer, filename.c_str());
    if (texture == nullptr) {
        std::cerr << "Error al cargar " << filename << ": " << IMG_GetError() << std::endl;
        return false;
    }

    textures[id] = texture;
    return true;
}

void TextureManager::register_grid_animation(const std::string& anim_id,
                                             const std::string& texture_id, int start_x,
                                             int start_y, int frame_width, int frame_height,
                                             int frame_count, uint32_t speed_ms) {

    AnimationClip clip;
    clip.frame_rate_ms = speed_ms;

    // Calculamos matemáticamente los rectángulos de colisión/recorte en la grilla
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

SDL_Texture* TextureManager::get_texture(const std::string& id) const {
    const auto it = textures.find(id);
    if (it == textures.end()) {
        throw std::runtime_error("Textura no encontrada: " + id);
    }
    return it->second;
}

const AnimationClip& TextureManager::get_animation(const std::string& anim_id) const {
    const auto it = animations.find(anim_id);
    if (it == animations.end()) {
        throw std::runtime_error("Animacion no encontrada: " + anim_id);
    }
    return it->second;
}

// Esta función determina qué cuadro (índice) de la animación corresponde dibujar ahora.
// Devuelve el índice del frame dentro del vector de la animación.
int TextureManager::get_current_animation_frame(const AnimationState& state,
                                                const AnimationClip& clip) {
    // 1. Calculamos cuánto tiempo pasó en milisegundos desde que empezó la animación
    uint32_t elapsed_time = SDL_GetTicks() - state.start_time;

    // 2. Calculamos cuántos cuadros totales ya deberían haber pasado
    // (Tiempo transcurrido dividido la velocidad de cada cuadro)
    int total_frames_elapsed = elapsed_time / clip.frame_rate_ms;

    int num_frames = clip.frames.size();
    if (num_frames == 0)
        return 0;

    if (state.is_looping) {
        // Si loopea (camina, respira), usamos el operador módulo % para reiniciar limpiamente
        return total_frames_elapsed % num_frames;
    } else {
        // Si es una acción de una sola vez (morir, atacar), se clava en el último cuadro
        if (total_frames_elapsed >= num_frames) {
            return num_frames - 1;
        }
        return total_frames_elapsed;
    }
}

void TextureManager::free_textures() {
    for (auto& pair: textures) {
        if (pair.second != nullptr) {
            SDL_DestroyTexture(pair.second);
        }
    }
    textures.clear();
}

TextureManager::~TextureManager() { free_textures(); }
