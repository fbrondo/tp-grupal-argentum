#pragma once

#include <SDL2pp/SDL2pp.hh>

#include "client/includes/texture_manager.h"

constexpr int PANEL_RIGHT_X = 686;
constexpr int PANEL_RIGHT_Y = 7;
constexpr int PANEL_RIGHT_W = 267;

constexpr int USER_INFO_Y = 7;
constexpr int USER_INFO_H = 114;

constexpr int INVENTORY_Y = 128;
constexpr int INVENTORY_W = 267;
constexpr int INVENTORY_H = 294;

constexpr int STATS_Y = 429;
constexpr int STATS_W = 267;
constexpr int STATS_H = 104;

constexpr int CONSOLE_X = 7;
constexpr int CONSOLE_Y = 7;
constexpr int CONSOLE_W = 672;
constexpr int CONSOLE_H = 135;

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
