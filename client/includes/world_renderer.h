#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <SDL2pp/Font.hh>
#include <SDL2pp/SDL2pp.hh>

#include "client/includes/client_protocol.h"
#include "client/includes/hud_renderer.h"
#include "client/includes/renderable_entity.h"
#include "client/includes/texture_manager.h"

class WorldRenderer {
private:
    SDL2pp::Renderer& renderer;
    TextureManager& texture_manager;
    HudRenderer hud_renderer;
    SDL2pp::Font level_font;
    SDL2pp::Font name_font;
    std::string local_player_name;
    uint32_t local_player_id;

    // El diccionario central que guarda TODAS las entidades visibles en el cliente
    // La clave (key) es el 'id' único que envía el servidor
    std::unordered_map<uint32_t, std::unique_ptr<RenderableEntity>> entities;
    std::unordered_set<uint32_t> static_entity_keys;
    std::optional<Map> current_map;
    SDL_Rect visible_map_bounds;

    // El rectángulo de la cámara (guarda x, y, w, h en píxeles del mundo)
    SDL_Rect camera;
    int camera_screen_offset_x;
    int camera_screen_offset_y;

    // Para centrar la cámara en el jugador principal
    void center_camera_on_player();
    void update_visible_map_bounds();

public:
    WorldRenderer(SDL2pp::Renderer& renderer_, TextureManager& texture_manager_);
    ~WorldRenderer() = default;

    WorldRenderer(const WorldRenderer&) = delete;
    WorldRenderer& operator=(const WorldRenderer&) = delete;

    void set_local_player(uint32_t id);
    void set_player_name(const std::string& name);
    void set_chat_bubble_on_local(const std::string& text);
    void update_hud_stats(const MsgPlayerStats& stats);
    void load_map(Map&& new_map, const std::vector<CitizenNpcSnapshot>& citizens);

    void add_chat_message(const std::string& msg);
    void update_chat_input(const std::string& buffer, bool is_active);
    bool is_point_inside_console(uint32_t x, uint32_t y) const;
    bool is_local_player_moving() const;
    // Procesa el snapshot recibido del servidor: actualiza posiciones o crea entidades nuevas
    void update_from_snapshot(const Snapshot& snapshot);

    // Corre el update de cada entidad individual usando el delta-time (dt)
    void update_animations(float dt);

    // Dibuja todo el mundo aplicando el Algoritmo del Pintor (Z-order por eje Y)
    void render();
};
