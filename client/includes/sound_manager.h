#pragma once

#include <SDL2/SDL_mixer.h>
#include <unordered_map>
#include <cstdint>
#include <string>
#include "common/includes/types.h"

class SoundManager {
private:
    // asocia el id del efecto con el chunk de SDL
    inline static std::unordered_map<SoundEffectID, Mix_Chunk*> sound_bank;

public:
    static void init();
    
    static void load_sound(SoundEffectID effect_id, const std::string& filepath);
    
    // Reproduce el sonido calculando el audio posicional
    static void play_effect(SoundEffectID effect_id, uint32_t fx_x, uint32_t fx_y, uint32_t player_x, uint32_t player_y);
    
    static void cleanup();
};