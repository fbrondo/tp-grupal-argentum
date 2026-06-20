#pragma once

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include <SDL2pp/SDL2pp.hh>

#include "client/includes/client_protocol.h"
#include "client/includes/font_manager.h"
#include "client/includes/hud_renderer.h"
#include "client/includes/renderable_entity.h"
#include "client/includes/texture_manager.h"
#include "common/includes/types.h"

class WorldRenderer {
private:
    struct ActiveVisualEffect {
        VisualEffectID effect_id;
        uint32_t pos_x;
        uint32_t pos_y;
        uint32_t start_time;
    };

    SDL2pp::Renderer& renderer;
    TextureManager& texture_manager;
    HudRenderer hud_renderer;
    FontManager& fonts;
    std::string local_player_name;
    uint32_t local_player_id;

    // El diccionario central que guarda TODAS las entidades visibles en el cliente
    // La clave (key) es el 'id' único que envía el servidor
    std::unordered_map<uint32_t, std::unique_ptr<RenderableEntity>> entities;
    std::unordered_set<uint32_t> static_entity_keys;
    std::vector<ActiveVisualEffect> active_visual_effects;
    std::optional<Map> current_map;
    SDL_Rect visible_map_bounds;
    int selected_npc_id = -1;

    // El rectángulo de la cámara (guarda x, y, w, h en píxeles del mundo)
    SDL_Rect camera;
    int camera_screen_offset_x;
    int camera_screen_offset_y;

    // Para centrar la cámara en el jugador principal
    void center_camera_on_player();
    void update_visible_map_bounds();

public:
    WorldRenderer(SDL2pp::Renderer& renderer_, TextureManager& texture_manager_,
                  FontManager& font_manager_);
    ~WorldRenderer() = default;

    WorldRenderer(const WorldRenderer&) = delete;
    WorldRenderer& operator=(const WorldRenderer&) = delete;

    void set_local_player(uint32_t id);
    void set_player_name(const std::string& name);
    void set_chat_bubble_on_local(const std::string& text);
    void set_chat_bubble_on_player(const std::string& player_name, const std::string& text);
    void update_hud_stats(const MsgPlayerStats& stats);
    void load_map(Map&& new_map, const std::vector<CitizenNpcSnapshot>& citizens);

    void add_chat_message(const std::string& msg, MessageColor color = COLOR_WHITE);
    void scroll_console(int delta);
    void update_chat_input(const std::string& buffer, bool is_active);
    void set_citizen_selected(int npc_id);
    bool is_point_inside_console(uint32_t x, uint32_t y) const;
    bool is_point_inside_console_input(uint32_t x, uint32_t y) const;
    bool is_local_player_moving() const;
    // Procesa el snapshot recibido del servidor: actualiza posiciones o crea entidades nuevas
    void update_from_snapshot(const Snapshot& snapshot);

    // Corre el update de cada entidad individual usando el delta-time (dt)
    void update_animations(float dt);

    // Dibuja todo el mundo aplicando el Algoritmo del Pintor (Z-order por eje Y)
    void render();

    // Dado un punto de pantalla, retorna el ID y tipo de la entidad que ocupa ese pixel.
    // Retorna nullopt si el punto no coincide con ninguna entidad (excluye items y jugador local).
    std::optional<std::pair<uint32_t, EntityType>> get_entity_at_screen(int screen_x,
                                                                        int screen_y) const;

    // Retorna el body_id (TypeNPC) de un NPC dado su server_id, o -1 si no existe.
    int get_npc_body_id(uint32_t server_id) const;
};
