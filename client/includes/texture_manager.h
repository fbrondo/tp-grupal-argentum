#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>
#include <unordered_map>
#include <vector>
#include <stdexcept>

// Estructura limpia para representar una animación de una acción específica
struct AnimationClip {
    std::vector<SDL_Rect> frames;
    uint32_t frame_rate_ms; // Velocidad de la animación en milisegundos por cuadro
};

struct AnimationState {
    std::string current_anim_id; // Ej: "player_walk_down"
    uint32_t start_time;         // Momento en que empezó la animación actual (SDL_GetTicks())
    bool is_looping;             // Si debe reiniciar al terminar o quedarse en el último cuadro
};

class TextureManager {
private:
    SDL_Renderer* renderer;
    
    // El diccionario centralizado de Texturas
    std::unordered_map<std::string, SDL_Texture*> textures;
    
    // Diccionario para mapear nombres de animaciones a sus respectivos cuadros/clips
    // Ejemplo clave: "player_walk_up", "player_idle_down"
    std::unordered_map<std::string, AnimationClip> animations;

    void free_textures();

public:
    explicit TextureManager(SDL_Renderer* renderer);
    ~TextureManager();

    // No queremos clonar el manejador por accidente (Buenas prácticas)
    TextureManager(const TextureManager&) = delete;
    TextureManager& operator=(const TextureManager&) = delete;

    // Cargar una textura base
    bool load_texture(const std::string& id, const std::string& filename);

    // Registrar una animación cortando el spritesheet por grilla estándar
    void register_grid_animation(const std::string& anim_id, const std::string& texture_id,
                                 int start_x, int start_y, 
                                 int frame_width, int frame_height, 
                                 int frame_count, uint32_t speed_ms);

    // Obtener la textura cruda si se necesita
    SDL_Texture* get_texture(const std::string& id) const;

    // Obtener los datos de un clip de animación
    const AnimationClip& get_animation(const std::string& anim_id) const;

    int get_current_animation_frame(const AnimationState& state, const AnimationClip& clip);
};
