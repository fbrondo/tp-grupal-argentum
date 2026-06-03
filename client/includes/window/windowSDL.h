#pragma once

#include <SDL2/SDL.h>
#include <SDL2pp/SDL2pp.hh>
#include "client/includes/window/window.h"

#define DRIVER_RENDERER -1
static constexpr int WINDOW_W = 800;
static constexpr int WINDOW_H = 600;

class WindowSDL: public Window {
private:
    SDL2pp::Window window;
    SDL2pp::Renderer renderer;

public:
    explicit WindowSDL(const char* title);
    ~WindowSDL() override;

    void clear() override;
    void present() override;
    SDL2pp::Renderer& get_renderer();
    void setIcon(const SDL2pp::Surface& icon);
};
