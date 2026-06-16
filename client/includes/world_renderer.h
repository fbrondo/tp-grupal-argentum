#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <SDL2pp/SDL2pp.hh>

#include "client/includes/client_protocol.h"
#include "client/includes/hud_renderer.h"
#include "client/includes/renderable_entity.h"
#include "client/includes/texture_manager.h"

static constexpr int TILE_SIZE = 32;

class WorldRenderer {
private:
    SDL2pp::Renderer& renderer;
    TextureManager& texture_manager;
    HudRenderer hud_renderer;

    // El diccionario central que guarda TODAS las entidades visibles en el cliente
    // La clave (key) es el 'id' único que envía el servidor
    std::unordered_map<uint32_t, std::unique_ptr<RenderableEntity>> entities;
    uint32_t local_player_id;
    std::optional<Map> current_map;

    // El rectángulo de la cámara (guarda x, y, w, h en píxeles del mundo)
    SDL_Rect camera;
    int camera_screen_offset_x;
    int camera_screen_offset_y;

    // Para centrar la cámara en el jugador principal
    void center_camera_on_player();

public:
    WorldRenderer(SDL2pp::Renderer& renderer_, TextureManager& texture_manager_);
    ~WorldRenderer() = default;

    WorldRenderer(const WorldRenderer&) = delete;
    WorldRenderer& operator=(const WorldRenderer&) = delete;

    void set_local_player(uint32_t id);
    void set_player_name(const std::string& name);
    void update_hud_stats(const MsgPlayerStats& stats);
    void load_map(Map&& new_map);

    void add_chat_message(const std::string& msg);
    void update_chat_input(const std::string& buffer, bool is_active);
    bool is_point_inside_console(uint32_t x, uint32_t y) const;
    // Procesa el snapshot recibido del servidor: actualiza posiciones o crea entidades nuevas
    void update_from_snapshot(const Snapshot& snapshot);

    // Corre el update de cada entidad individual usando el delta-time (dt)
    void update_animations(float dt);

    // Dibuja todo el mundo aplicando el Algoritmo del Pintor (Z-order por eje Y)
    void render();
};
