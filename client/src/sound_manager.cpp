#include "client/includes/sound_manager.h"


void SoundManager::init() {
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
}

void SoundManager::play_effect(SoundEffectID effect_id, uint32_t fx_x, uint32_t fx_y, uint32_t player_x, uint32_t player_y) {
    auto it = sound_bank.find(effect_id);
    Mix_Chunk* chunk_a_reproducir = nullptr;

    if (it == sound_bank.end()) {
        // No esta en RAM asi que lo cargamos por primera vez
        std::string filepath = "assets/audio/" + std::to_string(static_cast<uint16_t>(effect_id)) + ".wav";
        
        chunk_a_reproducir = Mix_LoadWAV(filepath.c_str());
        
        if (!chunk_a_reproducir) {
            // Si el archivo no existe en el disco no hacemos nada
            return; 
        }

        sound_bank[effect_id] = chunk_a_reproducir;
    } else {
        // Si ya existía usamos el puntero guardado
        chunk_a_reproducir = it->second;
    }

    if (chunk_a_reproducir) {
        int channel = Mix_PlayChannel(-1, chunk_a_reproducir, 0);
        if (channel != -1) {
        // Calculamos la distancia entre el personaje y el ruido
        int32_t dx = static_cast<int32_t>(fx_x) - static_cast<int32_t>(player_x);
        int32_t dy = static_cast<int32_t>(fx_y) - static_cast<int32_t>(player_y);
        int distance = static_cast<int>(std::sqrt(dx*dx + dy*dy) / 4);
        
        if (distance > 255) distance = 255;

        // Calculamos el ángulo (0 = Arriba, 90 = Derecha, 180 = Abajo, 270 = Izquierda)
        int angle = static_cast<int>(std::atan2(dx, -dy) * 180.0 / M_PI);
        if (angle < 0) angle += 360;

        //SDL_mixer altera el volumen de los canales izquierdo/derecho
        Mix_SetPosition(channel, angle, distance);
    }
    }
}

void SoundManager::cleanup() {
   for (auto& [id, chunk] : sound_bank) {
        if (chunk) Mix_FreeChunk(chunk);
    }
    sound_bank.clear();
    Mix_CloseAudio();
}