#pragma once

#include <cstdint>
#include <string>

#include <SDL2/SDL.h>
#include <SDL2pp/SDL2pp.hh>

#include "client/includes/core/constants.h"
#include "client/includes/texture_manager.h"
#include "common/includes/direction.h"

class RenderableEntity {
private:
    uint32_t id;
    std::string name;
    uint8_t level;
    EntityType type;
    bool is_short_race;

    int tile_x;
    int tile_y;

    float current_pixel_x;
    float current_pixel_y;

    Direction current_dir;
    bool is_moving;
    float movement_speed;

    uint16_t body_id;
    uint16_t head_id;
    uint8_t weapon_id;
    uint8_t shield_id;
    uint8_t helmet_id;
    uint8_t armor_id;
    bool is_ghost;

    uint16_t current_hp;
    uint16_t max_hp;

    std::string chat_bubble_text;
    uint32_t chat_bubble_start_ticks = 0;

    AnimationState anim_state;

    // --- Helpers --- //
    bool is_static_npc_type() const;
    std::string get_body_prefix() const;
    std::string get_body_anim_id(const std::string& action, Direction dir) const;
    int calculate_helmet_y_offset() const;

    // --- Render helpers --- //
    void render_item(SDL2pp::Renderer& renderer, TextureManager& texture_manager, int cam_x,
                     int cam_y, int offset_x, int offset_y);
    void render_npc_or_player(SDL2pp::Renderer& renderer, TextureManager& texture_manager,
                              int cam_x, int cam_y, int offset_x, int offset_y);
    void render_simple_layer(SDL2pp::Renderer& renderer, TextureManager& texture_manager,
                             const std::string& prefix, uint16_t item_id, const std::string& action,
                             uint32_t frame_index, const SDL_Rect& dst_rect, uint8_t alpha);
    void render_equipment_layers(SDL2pp::Renderer& renderer, TextureManager& texture_manager,
                                 const std::string& race_prefix, const std::string& action,
                                 uint32_t frame_index, const SDL_Rect& dst_rect, uint8_t alpha);

public:
    RenderableEntity(uint32_t id_, EntityType type_, int start_tile_x_, int start_tile_y_,
                     uint16_t body_id_, uint16_t head_id_, uint8_t weapon_id_, uint8_t shield_id_,
                     uint8_t helmet_id_ = 0, uint8_t armor_id_ = 0, uint8_t level_ = 0,
                     bool is_short_race_ = false);

    ~RenderableEntity() = default;

    RenderableEntity(const RenderableEntity&) = delete;
    RenderableEntity& operator=(const RenderableEntity&) = delete;

    // --- State updates --- //
    void update(float dt);
    void move_to(int target_tile_x, int target_tile_y, Direction dir);
    void set_equipment(uint8_t weapon_id_, uint8_t shield_id_, uint8_t helmet_id_,
                       uint8_t armor_id_);
    void set_ghost(const bool ghost_) { is_ghost = ghost_; }
    void set_name(const std::string& name_) { name = name_; }
    void set_level(uint8_t level_) { level = level_; }
    void set_hp(uint16_t current, uint16_t max) {
        current_hp = current;
        max_hp = max;
    }

    // --- Chat bubble --- //
    void set_chat_bubble(const std::string& text);
    bool has_active_chat_bubble() const;
    const std::string& get_chat_bubble_text() const { return chat_bubble_text; }
    uint32_t get_chat_bubble_start_ticks() const { return chat_bubble_start_ticks; }

    // --- Rendering --- //
    void render_with_camera(SDL2pp::Renderer& renderer, TextureManager& texture_manager, int cam_x,
                            int cam_y, int offset_x, int offset_y);

    // --- Getters --- //
    int get_tile_y() const { return tile_y; }
    float get_pixel_x() const { return current_pixel_x; }
    float get_pixel_y() const { return current_pixel_y; }
    uint32_t get_id() const { return id; }
    EntityType get_type() const { return type; }
    bool is_currently_moving() const { return is_moving; }
    uint8_t get_level() const { return level; }
    const std::string& get_name() const { return name; }
    uint16_t get_body_id() const { return body_id; }
    uint16_t get_current_hp() const { return current_hp; }
    uint16_t get_max_hp() const { return max_hp; }
};
