#include "client/includes/window/windowSDL.h"

#include <SDL2/SDL.h>

WindowSDL::WindowSDL(const char* title):
        window(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
               WINDOW_W,  // Ancho inicial
               WINDOW_H,  // Alto inicial
               SDL_WINDOW_SHOWN),
        renderer(window, DRIVER_RENDERER, SDL_RENDERER_ACCELERATED) {}

WindowSDL::~WindowSDL() = default;

void WindowSDL::clear() { renderer.Clear(); }

void WindowSDL::present() { renderer.Present(); }

SDL2pp::Renderer& WindowSDL::get_renderer() { return renderer; }

void WindowSDL::setIcon(const SDL2pp::Surface& icon) { window.SetIcon(icon); }
