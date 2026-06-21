#pragma once

#include <cstdint>
#include <string>

#include <SDL2/SDL.h>
#include <SDL2pp/SDL2pp.hh>

#include "client/includes/texture_manager.h"
#include "common/includes/direction.h"

enum class EntityType { PLAYER, NPC, ITEM, CITIZEN };

class RenderableEntity {
private:
    uint32_t id;
    EntityType type;
    bool is_short_race;
    uint8_t level;

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
    bool is_ghost;

    std::string name;

    std::string chat_bubble_text;
    uint32_t chat_bubble_start_ticks = 0;

    AnimationState anim_state;

public:
    RenderableEntity(uint32_t id_, EntityType type_, int start_tile_x_, int start_tile_y_,
                     uint16_t body_id_, uint16_t head_id_, uint8_t weapon_id_, uint8_t shield_id_,
                     uint8_t level_ = 0, bool is_short_race_ = false);

    virtual ~RenderableEntity() = default;

    RenderableEntity(const RenderableEntity&) = delete;
    RenderableEntity& operator=(const RenderableEntity&) = delete;

    // Actualiza la posición de interpolación y los cuadros visuales usando el delta-time (dt)
    void update(float dt);

    // Dibuja el esqueleto completo superponiendo las texturas correspondientes
    void render_with_camera(SDL2pp::Renderer& renderer, TextureManager& texture_manager, int cam_x,
                            int cam_y, int offset_x, int offset_y);

    // Método para cuando el servidor nos notifica una actualización de posición/dirección
    void move_to(int target_tile_x, int target_tile_y, Direction dir);
    void set_ghost(bool ghost);

    void set_name(const std::string& name_) { name = name_; }

    void set_chat_bubble(const std::string& text);
    bool has_active_chat_bubble() const;
    const std::string& get_chat_bubble_text() const { return chat_bubble_text; }
    uint32_t get_chat_bubble_start_ticks() const { return chat_bubble_start_ticks; }

    // Getters limpios y constantes para el ordenamiento Z (Algoritmo del Pintor)
    int get_tile_y() const { return tile_y; }
    float get_pixel_x() const { return current_pixel_x; }
    float get_pixel_y() const { return current_pixel_y; }
    uint32_t get_id() const { return id; }
    EntityType get_type() const { return type; }
    bool is_currently_moving() const { return is_moving; }
    uint8_t get_level() const { return level; }
    const std::string& get_name() const { return name; }
    uint16_t get_body_id() const { return body_id; }
};
