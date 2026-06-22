#pragma once

#include <deque>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include <SDL2pp/SDL2pp.hh>

#include "client/includes/chat_manager.h"
#include "client/includes/font_manager.h"
#include "client/includes/texture_manager.h"
#include "common/includes/protocol.h"
#include "common/includes/types.h"

class HudRenderer {
private:
    SDL2pp::Renderer& renderer;
    TextureManager& texture_manager;
    FontManager& fonts;
    int w_width;
    int w_height;
    MsgPlayerStats stats{};
    std::map<uint8_t, MsgSlot> inventory_slots;
    std::map<uint8_t, MsgSlot> equipped_inventory_slots;
    std::string player_name;
    std::unique_ptr<SDL2pp::Texture> player_name_texture;
    std::unique_ptr<SDL2pp::Texture> level_texture;
    std::unique_ptr<SDL2pp::Texture> hp_texture;
    std::unique_ptr<SDL2pp::Texture> mana_texture;
    std::unique_ptr<SDL2pp::Texture> exp_texture;
    std::unique_ptr<SDL2pp::Texture> safe_gold_texture;
    std::unique_ptr<SDL2pp::Texture> excess_gold_texture;
    std::unique_ptr<SDL2pp::Texture> resurrection_texture;
    std::deque<std::unique_ptr<SDL2pp::Texture>> chat_log_textures;
    std::unique_ptr<SDL2pp::Texture> chat_input_texture;
    uint16_t resurrection_time_left_ms = 0;
    bool chat_is_active = false;
    int console_scroll_offset = 0;
    std::optional<uint8_t> selected_slot;

    static constexpr size_t MAX_CHAT_LOG_SIZE = 50;

    void render_chat() const;
    void render_inventory() const;
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
    void scroll_console(int delta);
    bool is_point_inside_console(uint32_t x, uint32_t y) const;
    bool is_point_inside_console_input(uint32_t x, uint32_t y) const;
    std::optional<uint8_t> inventory_slot_at(uint32_t x, uint32_t y) const;
    std::optional<uint8_t> equipment_slot_at(uint32_t x, uint32_t y) const;
    void update_chat_input(const std::string& buffer, bool is_active);
    void update_stats(const MsgPlayerStats& new_stats);
    void update_inventory(const std::vector<MsgSlot>& slots);
    void update_equipment(const std::vector<MsgSlot>& slots);
    void update_resurrection_timer(uint16_t time_left_ms);
    void render_resurrection_notice() const;
    void set_selected_slot(std::optional<uint8_t> slot);
    void render() const;
};
