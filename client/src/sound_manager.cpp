#include "client/includes/sound_manager.h"

#include <cmath>
#include <iostream>

void SoundManager::init() {
    Mix_Init(MIX_INIT_MP3 | MIX_INIT_OGG);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
}

void SoundManager::play_background_music(const std::string& filepath) {
    stop_background_music();

    background_music = Mix_LoadMUS(filepath.c_str());

    if (!background_music) {
        std::cerr << "No se pudo cargar la musica: " << Mix_GetError() << std::endl;
        return;
    }

    if (Mix_PlayMusic(background_music, -1) == -1) {
        std::cerr << "Error al reproducir la musica: " << Mix_GetError() << std::endl;
    }
}

void SoundManager::stop_background_music() {
    if (background_music) {
        Mix_HaltMusic();
        Mix_FreeMusic(background_music);
        background_music = nullptr;
    }
}

void SoundManager::play_effect(SoundEffectID effect_id, uint32_t fx_x, uint32_t fx_y,
                               uint32_t player_x, uint32_t player_y) {
    auto it = sound_bank.find(effect_id);
    Mix_Chunk* chunk_a_reproducir = nullptr;

    if (it == sound_bank.end()) {
        // No esta en RAM asi que lo cargamos por primera vez
        std::string filepath = ARGENTUM_SHARE_PATH "/client/assets/Sounds/" +
                               std::to_string(static_cast<uint16_t>(effect_id)) + ".wav";

        chunk_a_reproducir = Mix_LoadWAV(filepath.c_str());

        if (!chunk_a_reproducir) {
            // Si el archivo no existe en el disco no hacemos nada
            std::cerr << "No se pudo cargar el sonido [" << filepath << "]: " << Mix_GetError()
                      << std::endl;
            return;
        }

        sound_bank[effect_id] = chunk_a_reproducir;
    } else {
        // Si ya existía usamos el puntero guardado
        chunk_a_reproducir = it->second;
    }

    const bool quiet_step = effect_id == SoundEffectID::PASO || effect_id == SoundEffectID::PASO_2;
    const bool quiet_resurrection = effect_id == SoundEffectID::RESUCITAR;
    const int volume = (quiet_step || quiet_resurrection) ? MIX_MAX_VOLUME / 3 : MIX_MAX_VOLUME;
    Mix_VolumeChunk(chunk_a_reproducir, volume);

    if (chunk_a_reproducir) {
        int channel = Mix_PlayChannel(-1, chunk_a_reproducir, 0);
        if (channel != -1) {
            // Calculamos la distancia entre el personaje y el ruido
            int32_t dx = static_cast<int32_t>(fx_x) - static_cast<int32_t>(player_x);
            int32_t dy = static_cast<int32_t>(fx_y) - static_cast<int32_t>(player_y);
            int distance = static_cast<int>(std::sqrt(dx * dx + dy * dy) / 4);

            if (distance > 255)
                distance = 255;

            // Calculamos el ángulo (0 = Arriba, 90 = Derecha, 180 = Abajo, 270 = Izquierda)
            int angle = static_cast<int>(std::atan2(dx, -dy) * 180.0 / M_PI);
            if (angle < 0)
                angle += 360;

            // SDL_mixer altera el volumen de los canales izquierdo/derecho
            Mix_SetPosition(channel, angle, distance);
        }
    }
}

void SoundManager::set_meditation_loop(bool active) {
    if (!active) {
        if (meditation_channel != -1)
            Mix_HaltChannel(meditation_channel);
        meditation_channel = -1;
        return;
    }
    if (meditation_channel != -1 && Mix_Playing(meditation_channel))
        return;

    const SoundEffectID effect_id = SoundEffectID::MEDITACION;
    auto it = sound_bank.find(effect_id);
    Mix_Chunk* meditation_sound = nullptr;
    if (it == sound_bank.end()) {
        const std::string filepath = ARGENTUM_SHARE_PATH "/client/assets/Sounds/158.wav";
        meditation_sound = Mix_LoadWAV(filepath.c_str());
        if (!meditation_sound) {
            std::cerr << "No se pudo cargar el sonido [" << filepath << "]: " << Mix_GetError()
                      << std::endl;
            return;
        }
        sound_bank[effect_id] = meditation_sound;
    } else {
        meditation_sound = it->second;
    }

    meditation_channel = Mix_PlayChannel(-1, meditation_sound, -1);
}

void SoundManager::cleanup() {
    set_meditation_loop(false);
    stop_background_music();
    for (auto& [id, chunk]: sound_bank) {
        if (chunk)
            Mix_FreeChunk(chunk);
    }
    sound_bank.clear();
    Mix_CloseAudio();
}
