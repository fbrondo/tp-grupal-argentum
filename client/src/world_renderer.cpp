#include "client/includes/world_renderer.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>
#include <limits>

#include "client/includes/chat_manager.h"
#include "client/includes/core/constants.h"
#include "client/includes/sound_manager.h"
#include "client/includes/world_renderer_utils.h"
#include "common/includes/types.h"

using world_renderer_utils::citizen_entity_key;
using world_renderer_utils::exp_next_level;
using world_renderer_utils::is_entity_inside_world_rect;
using world_renderer_utils::item_entity_key;
using world_renderer_utils::log_render_error_once;
using world_renderer_utils::npc_entity_key;
using world_renderer_utils::player_entity_key;
using world_renderer_utils::select_audible_sound_effects;
using world_renderer_utils::wrap_text;

WorldRenderer::WorldRenderer(SDL2pp::Renderer& renderer_, TextureManager& texture_manager_,
                             FontManager& font_manager_):
        renderer(renderer_),
        texture_manager(texture_manager_),
        hud_renderer(renderer, texture_manager, font_manager_, WINDOW_W, WINDOW_H),
        fonts(font_manager_),
        local_player_id(0),
        current_map(std::nullopt),
        visible_map_bounds{0, 0, 0, 0} {

    camera.x = 0;
    camera.y = 0;
    camera.w = 672;
    camera.h = 384;

    camera_screen_offset_x = 7;
    camera_screen_offset_y = 149;
}

void WorldRenderer::set_local_player(const uint32_t id) { local_player_id = id; }

void WorldRenderer::set_player_name(const std::string& name) {
    local_player_name = name;
    hud_renderer.set_player_name(name);
}

void WorldRenderer::set_chat_bubble_on_local(const std::string& text) {
    const uint32_t key = player_entity_key(local_player_id);
    auto it = entities.find(key);
    if (it != entities.end()) {
        it->second->set_chat_bubble(text);
    }
}

void WorldRenderer::set_chat_bubble_on_player(const std::string& player_name,
                                              const std::string& text) {
    for (auto& [key, entity]: entities) {
        if (entity->get_name() == player_name) {
            entity->set_chat_bubble(text);
            return;
        }
    }
}

void WorldRenderer::update_hud_stats(const MsgPlayerStats& stats) {
    hud_renderer.update_stats(stats);
}

void WorldRenderer::update_hud_inventory(const std::vector<MsgSlot>& inventory) {
    hud_renderer.update_inventory(inventory);
}

void WorldRenderer::update_hud_equipment(const std::vector<MsgSlot>& equipment) {
    hud_renderer.update_equipment(equipment);
}

void WorldRenderer::load_map(Map&& new_map, const std::vector<CitizenNpcSnapshot>& citizens) {
    entities.clear();
    static_entity_keys.clear();
    active_visual_effects.clear();
    current_map = std::move(new_map);
    camera.x = 0;
    camera.y = 0;
    update_visible_map_bounds();
    for (const auto& citizen: citizens) {
        const uint32_t entity_key = citizen_entity_key(citizen.id);
        entities[entity_key] = std::make_unique<RenderableEntity>(
                entity_key, EntityType::CITIZEN, citizen.position.x, citizen.position.y,
                citizen.type, 0, 0, 0);
        entities[entity_key]->move_to(citizen.position.x, citizen.position.y,
                                      static_cast<Direction>(citizen.direction));
        entities[entity_key]->set_name(std::string(citizen.name));
        static_entity_keys.insert(entity_key);
    }
}

void WorldRenderer::update_visible_map_bounds() {
    if (!current_map) {
        visible_map_bounds = {0, 0, 0, 0};
        return;
    }

    int min_x = current_map->width() * TILE_SIZE;
    int min_y = current_map->height() * TILE_SIZE;
    int max_x = -1;
    int max_y = -1;

    const std::array<std::pair<Layer, std::string>, 4> layers = {{
            {Layer::Background, "tile_bg_"},
            {Layer::Details, "tile_det_"},
            {Layer::Object, "tile_obj_"},
            {Layer::Roof, "tile_roof_"},
    }};

    for (auto& [layer, key_prefix]: layers) {
        for (int y = 0; y < current_map->height(); ++y) {
            for (int x = 0; x < current_map->width(); ++x) {
                const auto& tile_data = current_map->tile_at(x, y, layer);
                if (tile_data.sprite_id == 0) {
                    continue;
                }

                const std::string tex_key = key_prefix + std::to_string(tile_data.sprite_id);
                try {
                    SDL2pp::Texture& texture = texture_manager.get_texture(tex_key);
                    const int sprite_left = x * TILE_SIZE;
                    const int sprite_top = y * TILE_SIZE;
                    const int sprite_right = sprite_left + texture.GetWidth();
                    const int sprite_bottom = sprite_top + texture.GetHeight();
                    min_x = std::min(min_x, sprite_left);
                    min_y = std::min(min_y, sprite_top);
                    max_x = std::max(max_x, sprite_right);
                    max_y = std::max(max_y, sprite_bottom);
                } catch (const std::exception& e) {
                    log_render_error_once(tex_key, e);
                }
            }
        }
    }

    if (max_x < min_x || max_y < min_y) {
        visible_map_bounds = {0, 0, current_map->width() * TILE_SIZE,
                              current_map->height() * TILE_SIZE};
        return;
    }

    min_x = std::max(0, min_x);
    min_y = std::max(0, min_y);
    max_x = std::min(current_map->width() * TILE_SIZE, max_x);
    max_y = std::min(current_map->height() * TILE_SIZE, max_y);
    visible_map_bounds = {min_x, min_y, max_x - min_x, max_y - min_y};
}

void WorldRenderer::add_chat_message(const std::string& msg, const MessageColor color) {
    hud_renderer.add_chat_message(msg, color);
}

void WorldRenderer::scroll_console(int delta) { hud_renderer.scroll_console(delta); }

void WorldRenderer::update_chat_input(const std::string& buffer, bool is_active) {
    hud_renderer.update_chat_input(buffer, is_active);
}

void WorldRenderer::set_citizen_selected(int npc_id) { selected_npc_id = npc_id; }

bool WorldRenderer::is_point_inside_console(const uint32_t x, const uint32_t y) const {
    return hud_renderer.is_point_inside_console(x, y);
}

std::optional<uint8_t> WorldRenderer::inventory_slot_at(const uint32_t x, const uint32_t y) const {
    return hud_renderer.inventory_slot_at(x, y);
}

std::optional<uint8_t> WorldRenderer::equipment_slot_at(const uint32_t x, const uint32_t y) const {
    return hud_renderer.equipment_slot_at(x, y);
}

void WorldRenderer::set_selected_inv_slot(std::optional<uint8_t> slot) {
    hud_renderer.set_selected_slot(slot);
}

bool WorldRenderer::is_local_player_moving() const {
    const auto it = entities.find(player_entity_key(local_player_id));
    return it != entities.end() && it->second->is_currently_moving();
}

void WorldRenderer::center_camera_on_player() {
    if (!current_map)
        return;

    const auto it = entities.find(player_entity_key(local_player_id));
    if (it == entities.end())
        return;

    const float player_x = it->second->get_pixel_x();
    const float player_y = it->second->get_pixel_y();

    const int target_x = static_cast<int>(player_x) + (TILE_SIZE / 2) - (camera.w / 2);
    const int target_y = static_cast<int>(player_y) + (TILE_SIZE / 2) - (camera.h / 2);

    const int min_camera_x = visible_map_bounds.x;
    const int min_camera_y = visible_map_bounds.y;
    const int max_camera_x =
            std::max(min_camera_x, visible_map_bounds.x + visible_map_bounds.w - camera.w);
    const int max_camera_y =
            std::max(min_camera_y, visible_map_bounds.y + visible_map_bounds.h - camera.h);
    camera.x = std::clamp(target_x, min_camera_x, max_camera_x);
    camera.y = std::clamp(target_y, min_camera_y, max_camera_y);
}

// --- Snapshot Helpers ---

void WorldRenderer::update_players(const std::vector<PlayerSnapshotData>& players,
                                   std::vector<uint32_t>& ids_en_snapshot) {
    for (const auto& p_data: players) {
        const uint32_t entity_key = player_entity_key(p_data.id);
        ids_en_snapshot.push_back(entity_key);

        if (p_data.id == local_player_id) {
            SoundManager::set_meditation_loop((p_data.flags & PLAYER_FLAG_MEDITATING) != 0);
            MsgPlayerStats stats;
            stats.hp = p_data.stats.current_hp;
            stats.max_hp = p_data.stats.max_hp;
            stats.mana = p_data.stats.current_mana;
            stats.max_mana = p_data.stats.max_mana;
            stats.safe_gold = p_data.stats.safe_gold;
            stats.excess_gold = p_data.stats.excess_gold;
            stats.exp = p_data.stats.xp;
            stats.exp_next_level = exp_next_level(p_data.stats.level);
            stats.level = p_data.stats.level;
            hud_renderer.update_stats(stats);
            hud_renderer.update_resurrection_timer(p_data.resurrection_time_left_ms);
        }

        auto it = entities.find(entity_key);
        std::string player_name(p_data.name);
        if (it != entities.end()) {
            it->second->move_to(p_data.position.x, p_data.position.y,
                                static_cast<Direction>(p_data.direction));
            it->second->set_equipment(p_data.weapon_id, p_data.shield_id, p_data.helmet_id,
                                      p_data.armor_id);
            it->second->set_name(player_name);
            it->second->set_level(p_data.stats.level);
            it->second->set_ghost((p_data.flags & PLAYER_FLAG_GHOST) != 0);
        } else {
            bool is_short = (p_data.ch_traits.race == GNOME || p_data.ch_traits.race == DWARF);
            entities[entity_key] = std::make_unique<RenderableEntity>(
                    entity_key, EntityType::PLAYER, p_data.position.x, p_data.position.y,
                    p_data.ch_traits.body, p_data.ch_traits.head, p_data.weapon_id,
                    p_data.shield_id, p_data.helmet_id, p_data.armor_id, p_data.stats.level,
                    is_short);
            entities[entity_key]->set_name(player_name);
            entities[entity_key]->set_ghost((p_data.flags & PLAYER_FLAG_GHOST) != 0);
        }
    }
}

void WorldRenderer::update_npcs(const std::vector<NpcSnapshotData>& npcs,
                                std::vector<uint32_t>& ids_en_snapshot) {
    for (const auto& n_data: npcs) {
        const uint32_t entity_key = npc_entity_key(n_data.id);
        ids_en_snapshot.push_back(entity_key);
        auto it = entities.find(entity_key);
        if (it != entities.end()) {
            it->second->move_to(n_data.position.x, n_data.position.y,
                                static_cast<Direction>(n_data.direction));
            it->second->set_hp(n_data.current_hp, n_data.max_hp);
            it->second->set_name(std::string(n_data.name));
        } else {
            entities[entity_key] = std::make_unique<RenderableEntity>(
                    entity_key, EntityType::NPC, n_data.position.x, n_data.position.y,
                    static_cast<uint8_t>(n_data.type_id), 0, 0, 0);
            entities[entity_key]->move_to(n_data.position.x, n_data.position.y,
                                          static_cast<Direction>(n_data.direction));
            entities[entity_key]->set_name(std::string(n_data.name));
            entities[entity_key]->set_hp(n_data.current_hp, n_data.max_hp);
        }
    }
}

void WorldRenderer::update_floor_items(const std::vector<ItemGroundSnapshotData>& items,
                                       std::vector<uint32_t>& ids_en_snapshot) {
    for (const auto& i_data: items) {
        uint32_t item_client_id = item_entity_key(i_data.position.x, i_data.position.y);
        ids_en_snapshot.push_back(item_client_id);
        auto it = entities.find(item_client_id);
        if (it == entities.end()) {
            entities[item_client_id] = std::make_unique<RenderableEntity>(
                    item_client_id, EntityType::ITEM, i_data.position.x, i_data.position.y,
                    static_cast<uint8_t>(i_data.item_id), 0, 0, 0);
        }
    }
}

void WorldRenderer::cleanup_stale_entities(const std::vector<uint32_t>& ids_en_snapshot) {
    for (auto it = entities.begin(); it != entities.end();) {
        uint32_t entity_id = it->first;
        const bool sigue_viva = (std::find(ids_en_snapshot.begin(), ids_en_snapshot.end(),
                                           entity_id) != ids_en_snapshot.end());
        if (!sigue_viva && !static_entity_keys.contains(entity_id)) {
            it = entities.erase(it);
        } else {
            ++it;
        }
    }
}

void WorldRenderer::process_sound_effects(
        const std::vector<SoundEffectSnapshotData>& sound_effects) {
    const auto local_player = entities.find(player_entity_key(local_player_id));
    if (local_player == entities.end())
        return;

    const uint32_t player_x =
            static_cast<uint32_t>(local_player->second->get_pixel_x() / TILE_SIZE);
    const uint32_t player_y =
            static_cast<uint32_t>(local_player->second->get_pixel_y() / TILE_SIZE);
    const auto selected = select_audible_sound_effects(sound_effects, player_x, player_y);
    for (const auto& sound_effect: selected) {
        SoundManager::play_effect(sound_effect.effect_id, sound_effect.pos_x, sound_effect.pos_y,
                                  player_x, player_y);
    }
}

void WorldRenderer::update_from_snapshot(const Snapshot& snapshot) {
    std::vector<uint32_t> ids_en_snapshot;

    update_players(snapshot.players, ids_en_snapshot);
    update_npcs(snapshot.npcs, ids_en_snapshot);
    update_floor_items(snapshot.items_on_floor, ids_en_snapshot);
    cleanup_stale_entities(ids_en_snapshot);
    process_sound_effects(snapshot.sound_effects);

    const uint32_t now = SDL_GetTicks();
    for (const auto& visual_effect: snapshot.visual_effects) {
        if (visual_effect.recipient_id != 0 && visual_effect.recipient_id != local_player_id) {
            continue;
        }
        active_visual_effects.push_back(
                {visual_effect.effect_id, visual_effect.pos_x, visual_effect.pos_y, now});
    }
}

void WorldRenderer::update_animations(float dt) {
    for (auto& [id, entity]: entities) {
        entity->update(dt);
    }

    const uint32_t now = SDL_GetTicks();
    for (auto it = active_visual_effects.begin(); it != active_visual_effects.end();) {
        try {
            const VisualEffectClip& clip = texture_manager.get_visual_effect(it->effect_id);
            if (clip.frame_texture_ids.empty() || clip.frame_rate_ms == 0) {
                it = active_visual_effects.erase(it);
                continue;
            }
            const uint32_t elapsed = now - it->start_time;
            const uint32_t frame_index = elapsed / clip.frame_rate_ms;
            if (frame_index >= clip.frame_texture_ids.size()) {
                it = active_visual_effects.erase(it);
                continue;
            }
            ++it;
        } catch (const std::exception& e) {
            log_render_error_once("effect_" + std::to_string(static_cast<uint16_t>(it->effect_id)),
                                  e);
            it = active_visual_effects.erase(it);
        }
    }
}

// --- Render Helpers ---

void WorldRenderer::render_ground_layers(int start_tile_x, int end_tile_x, int start_tile_y,
                                         int end_tile_y) {
    std::array<std::pair<Layer, std::string>, 2> ground_layers = {{
            {Layer::Background, "tile_bg_"},
            {Layer::Object, "tile_obj_"},
    }};

    for (auto& [layer, key_prefix]: ground_layers) {
        for (int y = start_tile_y; y <= end_tile_y; ++y) {
            for (int x = start_tile_x; x <= end_tile_x; ++x) {
                auto& tile_data = current_map->tile_at(x, y, layer);
                uint32_t sprite_id = tile_data.sprite_id;
                if (sprite_id == 0)
                    continue;

                std::string tex_key = key_prefix + std::to_string(sprite_id);
                try {
                    SDL2pp::Texture& texture = texture_manager.get_texture(tex_key);
                    SDL_Rect dst;
                    dst.x = (x * TILE_SIZE) - camera.x + camera_screen_offset_x;
                    dst.y = (y * TILE_SIZE) - camera.y + camera_screen_offset_y;
                    dst.w = texture.GetWidth();
                    dst.h = texture.GetHeight();
                    renderer.Copy(texture, SDL2pp::NullOpt, SDL2pp::Rect(dst));
                } catch (const std::exception& e) {
                    log_render_error_once(tex_key, e);
                }
            }
        }
    }
}

void WorldRenderer::render_sorted_entities() {
    std::vector<std::pair<uint32_t, RenderableEntity*>> sorted_entities;
    sorted_entities.reserve(entities.size());
    for (const auto& [id, entity]: entities) {
        sorted_entities.push_back({id, entity.get()});
    }

    std::sort(sorted_entities.begin(), sorted_entities.end(),
              [](const std::pair<uint32_t, RenderableEntity*>& a,
                 const std::pair<uint32_t, RenderableEntity*>& b) {
                  if (a.second->get_pixel_y() != b.second->get_pixel_y())
                      return a.second->get_pixel_y() < b.second->get_pixel_y();
                  return a.first < b.first;
              });

    for (auto& [entity_key, entity]: sorted_entities) {
        entity->render_with_camera(renderer, texture_manager, camera.x, camera.y,
                                   camera_screen_offset_x, camera_screen_offset_y);

        if (entity->get_type() == EntityType::CITIZEN && selected_npc_id >= 0) {
            uint32_t citizen_server_id = entity_key - CITIZEN_ENTITY_OFFSET;
            if (static_cast<int>(citizen_server_id) == selected_npc_id) {
                uint16_t body = entity->get_body_id();
                const char* icon_key = nullptr;
                if (body == PRIEST)
                    icon_key = "npc_17";
                else if (body == BANKER)
                    icon_key = "npc_15";
                else if (body == MERCHANT)
                    icon_key = "npc_16";

                if (icon_key) {
                    const int ex = static_cast<int>(entity->get_pixel_x()) - camera.x +
                                   camera_screen_offset_x;
                    const int ey = static_cast<int>(entity->get_pixel_y()) - camera.y +
                                   camera_screen_offset_y + TILE_SIZE;
                    auto& icon = texture_manager.get_texture(icon_key);
                    const int icon_w = icon.GetWidth();
                    const int icon_h = icon.GetHeight();
                    renderer.Copy(icon, SDL2pp::NullOpt,
                                  SDL2pp::Rect(ex + TILE_SIZE / 2 - icon_w / 2, ey - icon_h - 4,
                                               icon_w, icon_h));
                }
            }
        }
    }
}

void WorldRenderer::render_entity_labels() {
    for (const auto& [entity_key, entity]: entities) {
        if (entity->get_type() != EntityType::PLAYER && entity->get_type() != EntityType::NPC &&
            entity->get_type() != EntityType::CITIZEN)
            continue;

        const int entity_screen_x = static_cast<int>(entity->get_pixel_x()) - camera.x +
                                    camera_screen_offset_x + TILE_SIZE / 2;
        const int entity_top_y =
                static_cast<int>(entity->get_pixel_y()) - camera.y + camera_screen_offset_y;
        int text_y = entity_top_y - 30;
        const bool is_local = (entity_key == player_entity_key(local_player_id));

        // HP bar for NPCs
        if (entity->get_type() == EntityType::NPC && entity->get_max_hp() > 0) {
            const uint16_t hp = entity->get_current_hp();
            const uint16_t max = entity->get_max_hp();
            const int bar_width = 60;
            const int bar_height = 6;
            const int bar_x = entity_screen_x - bar_width / 2;
            const int bar_y = text_y - bar_height - 4;

            renderer.SetDrawColor(0, 0, 0, 255);
            renderer.FillRect(SDL2pp::Rect(bar_x - 1, bar_y - 1, bar_width + 2, bar_height + 2));
            renderer.SetDrawColor(50, 50, 50, 255);
            renderer.FillRect(SDL2pp::Rect(bar_x, bar_y, bar_width, bar_height));

            const int fill_width = static_cast<int>(bar_width * hp / max);
            if (hp > max / 2)
                renderer.SetDrawColor(50, 200, 50, 255);
            else if (hp > max / 4)
                renderer.SetDrawColor(200, 200, 50, 255);
            else
                renderer.SetDrawColor(200, 50, 50, 255);
            renderer.FillRect(SDL2pp::Rect(bar_x, bar_y, fill_width, bar_height));

            renderer.SetDrawColor(0, 0, 0, 255);
            text_y = bar_y - 4;
        }

        // Level (players only)
        const uint8_t lvl = entity->get_level();
        if (entity->get_type() == EntityType::PLAYER && lvl > 0) {
            std::string lvl_str = "Nv. " + std::to_string(lvl);
            SDL2pp::Texture lvl_tex(renderer, fonts.get_level_font().RenderUTF8_Blended(
                                                      lvl_str, SDL_Color{200, 200, 200, 255}));
            const int lw = lvl_tex.GetWidth();
            const int lh = lvl_tex.GetHeight();
            text_y -= lh;
            renderer.Copy(lvl_tex, SDL2pp::NullOpt,
                          SDL2pp::Rect(entity_screen_x - lw / 2, text_y, lw, lh));
        }

        // Name
        if (is_local && !local_player_name.empty()) {
            SDL_Color name_color = {210, 170, 45, 255};
            SDL2pp::Texture name_tex(renderer, fonts.get_name_font().RenderUTF8_Blended(
                                                       local_player_name, name_color));
            const int nw = name_tex.GetWidth();
            const int nh = name_tex.GetHeight();
            text_y -= nh;
            renderer.Copy(name_tex, SDL2pp::NullOpt,
                          SDL2pp::Rect(entity_screen_x - nw / 2, text_y, nw, nh));
        } else if (!entity->get_name().empty()) {
            SDL_Color name_color = {255, 255, 255, 255};
            if (entity->get_type() == EntityType::NPC) {
                name_color = {255, 100, 100, 255};
            } else if (entity->get_type() == EntityType::CITIZEN) {
                name_color = {100, 200, 255, 255};
            }
            const int max_name_width = 100;
            auto name_lines =
                    wrap_text(fonts.get_npc_name_font(), entity->get_name(), max_name_width);
            for (auto it = name_lines.rbegin(); it != name_lines.rend(); ++it) {
                SDL2pp::Texture name_tex(
                        renderer, fonts.get_npc_name_font().RenderUTF8_Blended(*it, name_color));
                const int nw = name_tex.GetWidth();
                const int nh = name_tex.GetHeight();
                text_y -= nh;
                renderer.Copy(name_tex, SDL2pp::NullOpt,
                              SDL2pp::Rect(entity_screen_x - nw / 2, text_y, nw, nh));
            }
        }

        // Chat bubble (players only)
        if (entity->get_type() == EntityType::PLAYER && entity->has_active_chat_bubble()) {
            const uint32_t elapsed = SDL_GetTicks() - entity->get_chat_bubble_start_ticks();
            uint8_t alpha = 255;
            if (elapsed > 4000) {
                const uint32_t fade_elapsed = elapsed - 4000;
                alpha = static_cast<uint8_t>(255 - (255 * fade_elapsed / 1000));
            }
            SDL_Color bubble_color = {255, 255, 255, alpha};
            const int max_bubble_width = 150;
            auto bubble_lines = wrap_text(fonts.get_bubble_font(), entity->get_chat_bubble_text(),
                                          max_bubble_width);
            for (auto it = bubble_lines.rbegin(); it != bubble_lines.rend(); ++it) {
                SDL2pp::Texture bubble_tex(
                        renderer, fonts.get_bubble_font().RenderUTF8_Blended(*it, bubble_color));
                const int bw = bubble_tex.GetWidth();
                const int bh = bubble_tex.GetHeight();
                text_y -= bh;
                renderer.Copy(bubble_tex, SDL2pp::NullOpt,
                              SDL2pp::Rect(entity_screen_x - bw / 2, text_y, bw, bh));
            }
        }
    }
}

void WorldRenderer::render_details(int start_tile_x, int end_tile_x, int start_tile_y,
                                   int end_tile_y) {
    const RenderableEntity* local_player = nullptr;
    const auto local_player_it = entities.find(player_entity_key(local_player_id));
    if (local_player_it != entities.end()) {
        local_player = local_player_it->second.get();
    }

    for (int y = start_tile_y; y <= end_tile_y; ++y) {
        for (int x = start_tile_x; x <= end_tile_x; ++x) {
            const auto& tile_data = current_map->tile_at(x, y, Layer::Details);
            if (tile_data.sprite_id == 0)
                continue;

            const std::string tex_key = "tile_det_" + std::to_string(tile_data.sprite_id);
            try {
                SDL2pp::Texture& texture = texture_manager.get_texture(tex_key);
                const SDL_Rect detail_world_rect = {x * TILE_SIZE, y * TILE_SIZE,
                                                    texture.GetWidth(), texture.GetHeight()};
                const SDL_Rect dst = {(x * TILE_SIZE) - camera.x + camera_screen_offset_x,
                                      (y * TILE_SIZE) - camera.y + camera_screen_offset_y,
                                      texture.GetWidth(), texture.GetHeight()};
                if (local_player && is_entity_inside_world_rect(*local_player, detail_world_rect)) {
                    texture.SetAlphaMod(OCCLUDING_DETAIL_ALPHA);
                }
                renderer.Copy(texture, SDL2pp::NullOpt, SDL2pp::Rect(dst));
                texture.SetAlphaMod(OPAQUE_ALPHA);
            } catch (const std::exception& e) {
                log_render_error_once(tex_key, e);
            }
        }
    }
}

void WorldRenderer::render_roofs(int start_tile_x, int end_tile_x, int start_tile_y,
                                 int end_tile_y) {
    const RenderableEntity* local_player = nullptr;
    const auto local_player_it = entities.find(player_entity_key(local_player_id));
    if (local_player_it != entities.end()) {
        local_player = local_player_it->second.get();
    }

    for (int y = start_tile_y; y <= end_tile_y; ++y) {
        for (int x = start_tile_x; x <= end_tile_x; ++x) {
            auto& tile_data = current_map->tile_at(x, y, Layer::Roof);
            int32_t sprite_id = tile_data.sprite_id;
            if (sprite_id == 0)
                continue;

            std::string tex_key = "tile_roof_" + std::to_string(sprite_id);
            try {
                SDL2pp::Texture& texture = texture_manager.get_texture(tex_key);
                int tex_w = texture.GetWidth();
                int tex_h = texture.GetHeight();
                const SDL_Rect roof_world_rect = {x * TILE_SIZE, y * TILE_SIZE, tex_w, tex_h};
                const bool should_fade_roof =
                        local_player && is_entity_inside_world_rect(*local_player, roof_world_rect);

                SDL_Rect dst;
                dst.x = (x * TILE_SIZE) - camera.x + camera_screen_offset_x;
                dst.y = (y * TILE_SIZE) - camera.y + camera_screen_offset_y;
                dst.w = tex_w;
                dst.h = tex_h;

                if (should_fade_roof) {
                    texture.SetAlphaMod(OCCLUDING_ROOF_ALPHA);
                }
                renderer.Copy(texture, SDL2pp::NullOpt, SDL2pp::Rect(dst));
                texture.SetAlphaMod(OPAQUE_ALPHA);
            } catch (const std::exception& e) {
                log_render_error_once(tex_key, e);
            }
        }
    }
}

void WorldRenderer::render_active_visual_effects() {
    using world_renderer_utils::calculate_visual_effect_dst;

    for (const auto& effect: active_visual_effects) {
        try {
            const VisualEffectClip& clip = texture_manager.get_visual_effect(effect.effect_id);
            if (clip.frame_texture_ids.empty() || clip.frame_rate_ms == 0)
                continue;
            const uint32_t elapsed = SDL_GetTicks() - effect.start_time;
            const uint32_t frame_index = elapsed / clip.frame_rate_ms;
            if (frame_index >= clip.frame_texture_ids.size())
                continue;

            SDL2pp::Texture& texture =
                    texture_manager.get_texture(clip.frame_texture_ids.at(frame_index));
            SDL_Rect dst = calculate_visual_effect_dst(texture, effect.effect_id, effect.pos_x,
                                                       effect.pos_y, camera, camera_screen_offset_x,
                                                       camera_screen_offset_y);
            renderer.Copy(texture, SDL2pp::NullOpt, SDL2pp::Rect(dst));
        } catch (const std::exception& e) {
            log_render_error_once(
                    "effect_" + std::to_string(static_cast<uint16_t>(effect.effect_id)), e);
        }
    }
}

// Renderizamos con el Algoritmo de pintor (Z-Order por eje Y)
void WorldRenderer::render() {
    if (!current_map)
        return;

    center_camera_on_player();

    SDL_RenderSetClipRect(renderer.Get(), nullptr);
    hud_renderer.render();

    SDL_Rect view_rect = {camera_screen_offset_x, camera_screen_offset_y, camera.w, camera.h};
    SDL_RenderSetClipRect(renderer.Get(), &view_rect);

    int start_tile_x = std::max(0, camera.x / TILE_SIZE - CULLING_MARGIN_TILES);
    int end_tile_x =
            std::min(current_map->width() - 1,
                     (camera.x + camera.w + TILE_SIZE - 1) / TILE_SIZE + CULLING_MARGIN_TILES);
    int start_tile_y = std::max(0, camera.y / TILE_SIZE - CULLING_MARGIN_TILES);
    int end_tile_y =
            std::min(current_map->height() - 1,
                     (camera.y + camera.h + TILE_SIZE - 1) / TILE_SIZE + CULLING_MARGIN_TILES);

    render_ground_layers(start_tile_x, end_tile_x, start_tile_y, end_tile_y);

    SDL_RenderSetClipRect(renderer.Get(), &view_rect);
    render_sorted_entities();
    render_entity_labels();
    render_active_visual_effects();
    render_details(start_tile_x, end_tile_x, start_tile_y, end_tile_y);

    SDL_RenderSetClipRect(renderer.Get(), &view_rect);
    render_roofs(start_tile_x, end_tile_x, start_tile_y, end_tile_y);

    SDL_RenderSetClipRect(renderer.Get(), nullptr);
    hud_renderer.render_resurrection_notice();
}

std::optional<std::pair<uint32_t, EntityType>> WorldRenderer::get_entity_at_screen(
        int screen_x, int screen_y) const {
    std::optional<std::pair<uint32_t, EntityType>> closest_entity;
    int64_t closest_distance_squared = std::numeric_limits<int64_t>::max();

    for (const auto& [id, entity]: entities) {
        if (entity->get_type() == EntityType::ITEM)
            continue;
        int ex = static_cast<int>(entity->get_pixel_x()) - camera.x + camera_screen_offset_x;
        int ey = static_cast<int>(entity->get_pixel_y()) - camera.y + camera_screen_offset_y;
        constexpr int horizontal_margin = TILE_SIZE / 4;
        constexpr int upper_sprite_margin = TILE_SIZE / 2;
        if (screen_x >= ex - horizontal_margin && screen_x < ex + TILE_SIZE + horizontal_margin &&
            screen_y >= ey - upper_sprite_margin && screen_y < ey + TILE_SIZE) {
            EntityType type = entity->get_type();
            uint32_t server_id = id;
            if (type == EntityType::NPC)
                server_id = id - NPC_ENTITY_OFFSET;
            else if (type == EntityType::PLAYER)
                server_id = id - PLAYER_ENTITY_OFFSET;
            else if (type == EntityType::CITIZEN)
                server_id = id - CITIZEN_ENTITY_OFFSET;

            const int64_t dx = screen_x - (ex + TILE_SIZE / 2);
            const int64_t dy = screen_y - (ey + TILE_SIZE / 2);
            const int64_t distance_squared = dx * dx + dy * dy;
            if (distance_squared < closest_distance_squared) {
                closest_distance_squared = distance_squared;
                closest_entity = std::make_pair(server_id, type);
            }
        }
    }
    return closest_entity;
}

int WorldRenderer::get_npc_body_id(uint32_t server_id) const {
    const uint32_t entity_key = NPC_ENTITY_OFFSET + server_id;
    const auto it = entities.find(entity_key);
    if (it == entities.end() || it->second->get_type() != EntityType::NPC) {
        return -1;
    }
    return it->second->get_body_id();
}
