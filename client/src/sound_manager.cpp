#include "client/includes/sound_manager.h"

#include <iostream>

void SoundManager::init() {}

void SoundManager::play_background_music([[maybe_unused]] const std::string& filepath) {}

void SoundManager::stop_background_music() {}

void SoundManager::play_effect([[maybe_unused]] SoundEffectID effect_id,
                               [[maybe_unused]] uint32_t fx_x, [[maybe_unused]] uint32_t fx_y,
                               [[maybe_unused]] uint32_t player_x,
                               [[maybe_unused]] uint32_t player_y) {}

void SoundManager::cleanup() {}
