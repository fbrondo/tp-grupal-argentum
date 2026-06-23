#include "client/includes/input_utils.h"

#include <SDL2/SDL.h>

bool get_pressed_movement_direction(Direction& direction) {
    SDL_PumpEvents();
    if (SDL_GetKeyboardFocus() == nullptr) {
        return false;
    }

    const Uint8* keyboard_state = SDL_GetKeyboardState(nullptr);
    if (keyboard_state[SDL_SCANCODE_UP] || keyboard_state[SDL_SCANCODE_W]) {
        direction = UP;
        return true;
    }
    if (keyboard_state[SDL_SCANCODE_LEFT] || keyboard_state[SDL_SCANCODE_A]) {
        direction = LEFT;
        return true;
    }
    if (keyboard_state[SDL_SCANCODE_RIGHT] || keyboard_state[SDL_SCANCODE_D]) {
        direction = RIGHT;
        return true;
    }
    if (keyboard_state[SDL_SCANCODE_DOWN] || keyboard_state[SDL_SCANCODE_S]) {
        direction = DOWN;
        return true;
    }

    return false;
}
