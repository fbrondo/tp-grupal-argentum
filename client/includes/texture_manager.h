#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <SDL2/SDL.h>
#include <SDL2pp/SDL2pp.hh>

#include "client/includes/window/windowSDL.h"

// Estructura limpia para representar una animación de una acción específica
struct AnimationClip {
    std::vector<SDL_Rect> frames;
    uint32_t frame_rate_ms;  // Velocidad de la animación en milisegundos por cuadro
};

struct AnimationState {
    std::string current_anim_id;  // Ej: "player_walk_down"
    uint32_t start_time;          // Momento en que empezó la animación actual (SDL_GetTicks())
    bool is_looping;              // Si debe reiniciar al terminar o quedarse en el último cuadro
};

class TextureManager {
private:
    SDL2pp::Renderer& renderer;
    WindowSDL& window;

    // El diccionario centralizado de Texturas
    std::unordered_map<std::string, std::unique_ptr<SDL2pp::Texture>> textures;

    // Diccionario para mapear nombres de animaciones a sus respectivos cuadros/clips
    // Ejemplo clave: "player_walk_up", "player_idle_down"
    std::unordered_map<std::string, AnimationClip> animations;

public:
    explicit TextureManager(SDL2pp::Renderer& renderer_, WindowSDL& window_);
    ~TextureManager();

    TextureManager(const TextureManager&) = delete;
    TextureManager& operator=(const TextureManager&) = delete;

    // Cargar una textura base
    bool load_texture(const std::string& id, const std::string& filename);

    // Registrar una animación cortando el spritesheet por grilla estándar
    void register_grid_animation(const std::string& anim_id, /*const std::string& texture_id,*/
                                 int start_x, int start_y, int frame_width, int frame_height,
                                 int frame_count, uint32_t speed_ms);

    // Obtener la textura cruda si se necesita
    const SDL2pp::Texture& get_texture(const std::string& id) const;

    // Obtener los datos de un clip de animación
    const AnimationClip& get_animation(const std::string& anim_id) const;

    uint32_t get_current_animation_frame(const AnimationState& state, const AnimationClip& clip);
};
