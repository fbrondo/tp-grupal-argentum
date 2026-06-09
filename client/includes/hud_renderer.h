#pragma once

#include "client/includes/texture_manager.h"
#include <SDL2pp/SDL2pp.hh>

constexpr int PANEL_RIGHT_OFFSET = 250; // Cuánto espacio ocupa el panel derecho
constexpr int INVENTORY_Y = 10;
constexpr int INVENTORY_WIDTH = 240;
constexpr int INVENTORY_HEIGHT = 300;

constexpr int STATS_Y = 320;
constexpr int STATS_WIDTH = 240;
constexpr int STATS_HEIGHT = 150;

constexpr int CONSOLE_HEIGHT = 150;

class HudRenderer {
private:
    SDL2pp::Renderer& renderer;
    TextureManager& texture_manager;
    int w_width;
    int w_height;

public:
    HudRenderer(SDL2pp::Renderer& r, TextureManager& tm, int width, int height);
    ~HudRenderer() = default;
    void render() const;
};
