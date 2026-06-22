#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>

#include <SDL2/SDL_mixer.h>

#include "common/includes/types.h"

class SoundManager {
private:
    // asocia el id del efecto con el chunk de SDL
    inline static std::unordered_map<SoundEffectID, Mix_Chunk*> sound_bank;
    inline static int meditation_channel = -1;
    inline static Mix_Music* background_music = nullptr;
    inline static bool audio_open = false;

public:
    static bool init();

    static void load_sound(SoundEffectID effect_id, const std::string& filepath);

    // Reproduce el sonido calculando el audio posicional
    static void play_effect(SoundEffectID effect_id, uint32_t fx_x, uint32_t fx_y,
                            uint32_t player_x, uint32_t player_y);
    static void set_meditation_loop(bool active);
    static void play_background_music(const std::string& filepath,
                                      double start_position_seconds = 0.0);
    static void stop_background_music();
    static void cleanup();
};
