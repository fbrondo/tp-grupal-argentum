#pragma once

#include <deque>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include <SDL2pp/Font.hh>
#include <SDL2pp/SDL2pp.hh>

#include "client/includes/texture_manager.h"
#include "common/includes/protocol.h"

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
constexpr int PROGRESS_BAR_X = PANEL_RIGHT_X + 20;
constexpr int PROGRESS_BAR_W = 227;
constexpr int PROGRESS_BAR_H = 20;
constexpr int HP_BAR_Y = STATS_Y + 27;
constexpr int MANA_BAR_Y = STATS_Y + 72;
constexpr int EXP_BAR_Y = USER_INFO_Y + 82;
constexpr int LEVEL_X = PANEL_RIGHT_X + 27;
constexpr int LEVEL_Y = USER_INFO_Y + 12;
constexpr int LEVEL_W = 36;
constexpr int LEVEL_H = 36;
constexpr int NAME_X = PANEL_RIGHT_X + 70;
constexpr int NAME_Y = USER_INFO_Y + 12;
constexpr int NAME_W = 177;
constexpr int NAME_H = 36;

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
    MsgPlayerStats stats{};
    std::map<uint8_t, MsgSlot> inventory_slots;
    std::map<uint8_t, MsgSlot> equipped_inventory_slots;
    SDL2pp::Font font;
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
    void render_inventory() const;
    void render_progress_bar(int x, int y, int width, int height, uint32_t current,
                             uint32_t maximum, SDL_Color color) const;
    std::unique_ptr<SDL2pp::Texture> create_text_texture(const std::string& text);
    void render_centered_text(const std::unique_ptr<SDL2pp::Texture>& texture, int x, int y,
                              int width, int height) const;
    void update_player_textures();
    void update_stats_textures();

public:
    HudRenderer(SDL2pp::Renderer& r, TextureManager& tm, int width, int height);
    ~HudRenderer() = default;
    void set_player_name(const std::string& name);
    void add_chat_message(const std::string& msg);
    bool is_point_inside_console(uint32_t x, uint32_t y) const;
    std::optional<uint8_t> inventory_slot_at(uint32_t x, uint32_t y) const;
    std::optional<uint8_t> equipment_slot_at(uint32_t x, uint32_t y) const;
    void update_chat_input(const std::string& buffer, bool is_active);
    void update_stats(const MsgPlayerStats& new_stats);
    void update_inventory(const std::vector<MsgSlot>& slots);
    void update_equipment(const std::vector<MsgSlot>& slots);
    void update_resurrection_timer(uint16_t time_left_ms);
    void render_resurrection_notice() const;
    void render() const;
};
