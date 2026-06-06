#pragma once

#include <string>
#include <cstdint>
#include <SDL2/SDL.h>
#include <SDL2pp/SDL2pp.hh>

#include "common/includes/direction.h"
#include "client/includes/texture_manager.h"

class RenderableEntity {
private:
    uint32_t id;

    int tile_x;
    int tile_y;

    float current_pixel_x;
    float current_pixel_y;

    Direction current_dir;
    bool is_moving;
    float movement_speed;

    uint8_t body_id;
    uint8_t head_id;
    uint8_t weapon_id;
    uint8_t shield_id;

    AnimationState anim_state;

public:
    RenderableEntity(uint32_t id, int start_tile_x, int start_tile_y,
                     uint8_t body_id_, uint8_t head_id_, uint8_t weapon_id_, uint8_t shield_id_);

    virtual ~RenderableEntity() = default;

    RenderableEntity(const RenderableEntity&) = delete;
    RenderableEntity& operator=(const RenderableEntity&) = delete;

    // Actualiza la posición de interpolación y los cuadros visuales usando el delta-time (dt)
    void update(float dt);

    // Dibuja el esqueleto completo superponiendo las texturas correspondientes
    void render_with_camera(SDL2pp::Renderer& renderer, TextureManager& texture_manager, int cam_x, int cam_y, int offset_x, int offset_y);

    // Método para cuando el servidor nos notifica una actualización de posición/dirección
    void move_to(int target_tile_x, int target_tile_y, Direction dir);

    // Getters limpios y constantes para el ordenamiento Z (Algoritmo del Pintor)
    int get_tile_y() const { return tile_y; }
    float get_pixel_x() const { return current_pixel_x; }
    float get_pixel_y() const { return current_pixel_y; }
    uint32_t get_id() const { return id; }
};