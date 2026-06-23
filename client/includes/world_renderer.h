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
#include "client/includes/core/visual_effect.h"
#include "client/includes/font_manager.h"
#include "client/includes/hud_renderer.h"
#include "client/includes/renderable_entity.h"
#include "client/includes/texture_manager.h"
#include "common/includes/core/snapshot.h"
#include "common/includes/types.h"

class WorldRenderer {
private:
    SDL2pp::Renderer& renderer;
    TextureManager& texture_manager;
    HudRenderer hud_renderer;
    FontManager& fonts;
    std::string local_player_name;
    uint32_t local_player_id;

    std::unordered_map<uint32_t, std::unique_ptr<RenderableEntity>> entities;
    std::unordered_set<uint32_t> static_entity_keys;
    std::vector<ActiveVisualEffect> active_visual_effects;
    std::optional<Map> current_map;
    SDL_Rect visible_map_bounds;
    int selected_npc_id = -1;

    SDL_Rect camera;
    int camera_screen_offset_x;
    int camera_screen_offset_y;

    void center_camera_on_player();
    void update_visible_map_bounds();

    // Render helpers (sub-methods de render())
    void render_ground_layers(int start_tile_x, int end_tile_x, int start_tile_y, int end_tile_y);
    void render_sorted_entities();
    void render_entity_labels();
    void render_details(int start_tile_x, int end_tile_x, int start_tile_y, int end_tile_y);
    void render_roofs(int start_tile_x, int end_tile_x, int start_tile_y, int end_tile_y);
    void render_active_visual_effects();

    // Snapshot helpers (sub-methods de update_from_snapshot())
    void update_players(const std::vector<PlayerSnapshotData>& players,
                        std::vector<uint32_t>& ids_en_snapshot);
    void update_npcs(const std::vector<NpcSnapshotData>& npcs,
                     std::vector<uint32_t>& ids_en_snapshot);
    void update_floor_items(const std::vector<ItemGroundSnapshotData>& items,
                            std::vector<uint32_t>& ids_en_snapshot);
    void cleanup_stale_entities(const std::vector<uint32_t>& ids_en_snapshot);
    void process_sound_effects(const std::vector<SoundEffectSnapshotData>& sound_effects);

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
    void update_hud_inventory(const std::vector<MsgSlot>& inventory);
    void update_hud_equipment(const std::vector<MsgSlot>& equipment);
    void load_map(Map&& new_map, const std::vector<CitizenNpcSnapshot>& citizens);

    void add_chat_message(const std::string& msg, MessageColor color = COLOR_WHITE);
    void scroll_console(int delta);
    void update_chat_input(const std::string& buffer, bool is_active);
    void set_citizen_selected(int npc_id);
    bool is_point_inside_console(uint32_t x, uint32_t y) const;
    std::optional<uint8_t> inventory_slot_at(uint32_t x, uint32_t y) const;
    std::optional<uint8_t> equipment_slot_at(uint32_t x, uint32_t y) const;
    void set_selected_inv_slot(std::optional<uint8_t> slot);
    bool is_local_player_moving() const;
    std::optional<std::pair<uint32_t, EntityType>> get_entity_at_screen(int screen_x,
                                                                        int screen_y) const;
    int get_npc_body_id(uint32_t server_id) const;
    void update_from_snapshot(const Snapshot& snapshot);
    void update_animations(float dt);
    void render();
};
