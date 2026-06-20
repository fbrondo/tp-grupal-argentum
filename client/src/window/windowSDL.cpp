#include "client/includes/window/windowSDL.h"

#include <SDL2/SDL.h>

#include "client/includes/core/constants.h"

WindowSDL::WindowSDL(const char* title):
        window(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
               WINDOW_W,  // Ancho inicial
               WINDOW_H,  // Alto inicial
               SDL_WINDOW_RESIZABLE),
        renderer(window, DRIVER_RENDERER, SDL_RENDERER_ACCELERATED) {
    SDL_RenderSetLogicalSize(renderer.Get(), WINDOW_W, WINDOW_H);
}

WindowSDL::WindowSDL(const char* title, int width, int height, bool fullscreen):
        window(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height,
               SDL_WINDOW_RESIZABLE | (fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0)),
        renderer(window, DRIVER_RENDERER, SDL_RENDERER_ACCELERATED) {
    SDL_RenderSetLogicalSize(renderer.Get(), WINDOW_W, WINDOW_H);
}

WindowSDL::~WindowSDL() = default;

void WindowSDL::clear() { renderer.Clear(); }

void WindowSDL::present() { renderer.Present(); }

SDL2pp::Renderer& WindowSDL::get_renderer() { return renderer; }

void WindowSDL::setIcon(const SDL2pp::Surface& icon) { window.SetIcon(icon); }

void WindowSDL::toggle_fullscreen() {
    const Uint32 flags = SDL_GetWindowFlags(window.Get());
    const bool is_fullscreen = (flags & SDL_WINDOW_FULLSCREEN_DESKTOP) != 0;
    SDL_SetWindowFullscreen(window.Get(), is_fullscreen ? 0 : SDL_WINDOW_FULLSCREEN_DESKTOP);
}
