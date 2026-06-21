#pragma once

#include <string>
#include <vector>

#include <SDL2/SDL.h>
#include <stdint.h>

struct AnimationClip {
    std::vector<SDL_Rect> frames;
    uint32_t frame_rate_ms;
};

struct AnimationState {
    std::string current_anim_id;
    uint32_t start_time;
    bool is_looping;
};
