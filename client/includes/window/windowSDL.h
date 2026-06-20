#pragma once

#include <SDL2/SDL.h>
#include <SDL2pp/SDL2pp.hh>

#include "client/includes/window/window.h"

class WindowSDL: public Window {
private:
    SDL2pp::Window window;
    SDL2pp::Renderer renderer;

public:
    explicit WindowSDL(const char* title);
    WindowSDL(const char* title, int width, int height, bool fullscreen);
    ~WindowSDL() override;

    void clear() override;
    void present() override;
    SDL2pp::Renderer& get_renderer();
    void setIcon(const SDL2pp::Surface& icon);
    void toggle_fullscreen() override;
};
