#include "client/includes/sound_manager.h"

#include <algorithm>
#include <cmath>
#include <iostream>

namespace {
constexpr double POSITIONAL_ATTENUATION_PER_TILE = 16.0;
}

bool SoundManager::init() {
    const int requested_codecs = MIX_INIT_MP3 | MIX_INIT_OGG;
    const int initialized_codecs = Mix_Init(requested_codecs);
    if ((initialized_codecs & MIX_INIT_MP3) == 0) {
        std::cerr << "No se pudo inicializar el soporte MP3: " << Mix_GetError() << std::endl;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1) {
        std::cerr << "No se pudo abrir el dispositivo de audio: " << Mix_GetError() << std::endl;
        Mix_Quit();
        return false;
    }

    audio_open = true;
    return true;
}

void SoundManager::play_background_music(const std::string& filepath,
                                         const double start_position_seconds) {
    if (!audio_open)
        return;

    stop_background_music();

    background_music = Mix_LoadMUS(filepath.c_str());

    if (!background_music) {
        std::cerr << "No se pudo cargar la musica: " << Mix_GetError() << std::endl;
        return;
    }

    if (Mix_PlayMusic(background_music, -1) == -1) {
        std::cerr << "Error al reproducir la musica: " << Mix_GetError() << std::endl;
        return;
    }

    if (start_position_seconds > 0.0 && Mix_SetMusicPosition(start_position_seconds) == -1) {
        std::cerr << "No se pudo adelantar la musica: " << Mix_GetError() << std::endl;
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
            const int64_t dx = static_cast<int64_t>(fx_x) - static_cast<int64_t>(player_x);
            const int64_t dy = static_cast<int64_t>(fx_y) - static_cast<int64_t>(player_y);
            const int distance = std::min(
                    255,
                    static_cast<int>(std::hypot(static_cast<double>(dx), static_cast<double>(dy)) *
                                     POSITIONAL_ATTENUATION_PER_TILE));

            // Calculamos el ángulo (0 = Arriba, 90 = Derecha, 180 = Abajo, 270 = Izquierda)
            int angle = static_cast<int>(
                    std::atan2(static_cast<double>(dx), static_cast<double>(-dy)) * 180.0 / M_PI);
            if (angle < 0)
                angle += 360;

            // SDL_mixer altera el volumen de los canales izquierdo/derecho
            if (Mix_SetPosition(channel, static_cast<Sint16>(angle),
                                static_cast<Uint8>(distance)) == 0) {
                std::cerr << "No se pudo posicionar el sonido: " << Mix_GetError() << std::endl;
            }
        } else {
            std::cerr << "No hay un canal disponible para reproducir el sonido: " << Mix_GetError()
                      << std::endl;
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
    if (audio_open) {
        Mix_CloseAudio();
        audio_open = false;
    }
    Mix_Quit();
}
