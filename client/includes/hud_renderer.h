#pragma once

#include <deque>
#include <memory>
#include <string>

#include <SDL2pp/SDL2pp.hh>

#include "client/includes/chat_manager.h"
#include "client/includes/font_manager.h"
#include "client/includes/texture_manager.h"
#include "common/includes/protocol.h"

class HudRenderer {
private:
    SDL2pp::Renderer& renderer;
    TextureManager& texture_manager;
    FontManager& fonts;
    int w_width;
    int w_height;
    MsgPlayerStats stats{};
    std::string player_name;
    std::unique_ptr<SDL2pp::Texture> player_name_texture;
    std::unique_ptr<SDL2pp::Texture> level_texture;
    std::unique_ptr<SDL2pp::Texture> hp_texture;
    std::unique_ptr<SDL2pp::Texture> mana_texture;
    std::unique_ptr<SDL2pp::Texture> exp_texture;
    std::unique_ptr<SDL2pp::Texture> resurrection_texture;
    std::deque<std::unique_ptr<SDL2pp::Texture>> chat_log_textures;
    std::unique_ptr<SDL2pp::Texture> chat_input_texture;
    uint16_t resurrection_time_left_ms = 0;
    bool chat_is_active = false;

    void render_chat() const;
    void render_progress_bar(int x, int y, int width, int height, uint32_t current,
                             uint32_t maximum, SDL_Color color) const;
    std::unique_ptr<SDL2pp::Texture> create_text_texture(const std::string& text);
    std::unique_ptr<SDL2pp::Texture> create_input_texture(const std::string& text);
    std::unique_ptr<SDL2pp::Texture> create_hud_texture(const std::string& text);
    std::unique_ptr<SDL2pp::Texture> create_text_texture_colored(const std::string& text,
                                                                 SDL_Color color);
    void render_centered_text(const std::unique_ptr<SDL2pp::Texture>& texture, int x, int y,
                              int width, int height) const;
    void update_player_textures();
    void update_stats_textures();

public:
    HudRenderer(SDL2pp::Renderer& r, TextureManager& tm, FontManager& fm, int width, int height);
    ~HudRenderer() = default;
    void set_player_name(const std::string& name);
    void add_chat_message(const std::string& msg, MessageColor color = COLOR_WHITE);
    bool is_point_inside_console(uint32_t x, uint32_t y) const;
    void update_chat_input(const std::string& buffer, bool is_active);
    void update_stats(const MsgPlayerStats& new_stats);
    void update_resurrection_timer(uint16_t time_left_ms);
    void render_resurrection_notice() const;
    void render() const;
};
