#include "client/includes/world_renderer.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>
#include <limits>
#include <sstream>
#include <unordered_set>

#include "client/includes/chat_manager.h"
#include "client/includes/core/constants.h"
#include "client/includes/sound_manager.h"
#include "common/includes/types.h"

static uint32_t player_entity_key(uint32_t server_id) { return PLAYER_ENTITY_OFFSET + server_id; }

static uint32_t npc_entity_key(uint32_t server_id) { return NPC_ENTITY_OFFSET + server_id; }

static uint32_t citizen_entity_key(uint32_t server_id) { return CITIZEN_ENTITY_OFFSET + server_id; }

static uint32_t item_entity_key(uint32_t pos_x, uint32_t pos_y) {
    return ITEM_ENTITY_OFFSET + (pos_x * 1000) + pos_y;
}

static constexpr uint8_t OCCLUDING_DETAIL_ALPHA = 150;
static constexpr uint8_t OCCLUDING_ROOF_ALPHA = 120;
static constexpr uint8_t OPAQUE_ALPHA = 255;

static uint32_t exp_next_level(uint8_t level) {
    return static_cast<uint32_t>(1000 * std::pow(level, 1.8));
}

struct SoundEffectCandidate {
    SoundEffectSnapshotData effect;
    int priority;
    int64_t distance_sq;
};

static int64_t squared_distance(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2) {
    const int64_t dx = static_cast<int64_t>(x1) - static_cast<int64_t>(x2);
    const int64_t dy = static_cast<int64_t>(y1) - static_cast<int64_t>(y2);
    return dx * dx + dy * dy;
}

static bool is_entity_inside_world_rect(const RenderableEntity& entity, const SDL_Rect& rect) {
    const SDL_Point entity_center = {
            static_cast<int>(entity.get_pixel_x()) + TILE_SIZE / 2,
            static_cast<int>(entity.get_pixel_y()) + TILE_SIZE / 2,
    };
    return SDL_PointInRect(&entity_center, &rect);
}

static int sound_effect_priority(SoundEffectID effect_id) {
    static constexpr std::array<std::pair<SoundEffectID, int>, 23> SOUND_PRIORITIES = {{
            {SoundEffectID::MUERTE_HOMBRE, 4},
            {SoundEffectID::RESUCITAR, 4},
            {SoundEffectID::RESUCITAR_SACERDOTE, 4},
            {SoundEffectID::GOLPE_RECIBIDO, 3},
            {SoundEffectID::GOLPE_ARMA, 3},
            {SoundEffectID::ESPADAZO, 3},
            {SoundEffectID::FLECHA, 3},
            {SoundEffectID::FLECHA_MAGICA, 3},
            {SoundEffectID::RESORTE_EXPLOSIVO, 3},
            {SoundEffectID::CURAR, 2},
            {SoundEffectID::CURAR_2, 2},
            {SoundEffectID::TOMAR_POCION, 2},
            {SoundEffectID::EQUIPAR_ARMA, 2},
            {SoundEffectID::DROP_ESPECIAL_NPC, 2},
            {SoundEffectID::PASO, 0},
            {SoundEffectID::PASO_2, 0},
            {SoundEffectID::PASOS, 0},
            {SoundEffectID::PASOS_EN_GRAVA, 0},
            {SoundEffectID::PASO_3, 0},
            {SoundEffectID::PASO_4, 0},
            {SoundEffectID::PASO_5, 0},
            {SoundEffectID::PASO_6, 0},
            {SoundEffectID::PASO_7, 0},
    }};

    const auto it =
            std::find_if(SOUND_PRIORITIES.begin(), SOUND_PRIORITIES.end(),
                         [effect_id](const auto& priority) { return priority.first == effect_id; });
    if (it != SOUND_PRIORITIES.end()) {
        return it->second;
    }
    return 1;
}

static bool is_same_sound_nearby(const SoundEffectSnapshotData& first,
                                 const SoundEffectSnapshotData& second, int64_t max_distance_sq) {
    return first.effect_id == second.effect_id &&
           squared_distance(first.pos_x, first.pos_y, second.pos_x, second.pos_y) <=
                   max_distance_sq;
}

static bool was_similar_sound_already_selected(const std::vector<SoundEffectSnapshotData>& selected,
                                               const SoundEffectSnapshotData& candidate,
                                               int64_t max_distance_sq) {
    for (const auto& selected_effect: selected) {
        if (is_same_sound_nearby(selected_effect, candidate, max_distance_sq)) {
            return true;
        }
    }
    return false;
}

// Agrupo sonidos iguales que ocurren muy cerca uno del otro para no saturar.
// Si quedan demasiados, priorizo los mas importantes y cercanos.
static std::vector<SoundEffectSnapshotData> select_audible_sound_effects(
        const std::vector<SoundEffectSnapshotData>& effects, uint32_t player_x, uint32_t player_y) {
    constexpr size_t MAX_SOUNDS_PER_SNAPSHOT = 8;
    constexpr int64_t SAME_SOUND_GROUP_RADIUS_TILES = 2;
    constexpr int64_t SAME_SOUND_GROUP_RADIUS_SQ =
            SAME_SOUND_GROUP_RADIUS_TILES * SAME_SOUND_GROUP_RADIUS_TILES;

    std::vector<SoundEffectCandidate> candidates;
    candidates.reserve(effects.size());
    for (const auto& effect: effects) {
        candidates.push_back({effect, sound_effect_priority(effect.effect_id),
                              squared_distance(effect.pos_x, effect.pos_y, player_x, player_y)});
    }

    std::sort(candidates.begin(), candidates.end(),
              [](const SoundEffectCandidate& a, const SoundEffectCandidate& b) {
                  if (a.priority != b.priority) {
                      return a.priority > b.priority;
                  }
                  return a.distance_sq < b.distance_sq;
              });

    std::vector<SoundEffectSnapshotData> selected;
    selected.reserve(std::min(MAX_SOUNDS_PER_SNAPSHOT, effects.size()));

    for (const auto& candidate: candidates) {
        if (was_similar_sound_already_selected(selected, candidate.effect,
                                               SAME_SOUND_GROUP_RADIUS_SQ)) {
            continue;
        }

        selected.push_back(candidate.effect);
        if (selected.size() == MAX_SOUNDS_PER_SNAPSHOT) {
            break;
        }
    }

    return selected;
}

static void log_render_error_once(const std::string& texture_key, const std::exception& e) {
    static std::unordered_set<std::string> logged_errors;
    if (logged_errors.insert(texture_key).second) {
        std::cerr << "[WorldRenderer] No se pudo renderizar " << texture_key << ": " << e.what()
                  << std::endl;
    }
}

static std::vector<std::string> wrap_text(SDL2pp::Font& font, const std::string& text,
                                          int max_width) {
    std::vector<std::string> lines;
    if (text.empty())
        return lines;

    std::istringstream stream(text);
    std::string word;
    std::string current_line;

    while (stream >> word) {
        std::string test_line = current_line.empty() ? word : current_line + " " + word;
        SDL2pp::Surface surface = font.RenderUTF8_Blended(test_line, SDL_Color{255, 255, 255, 255});
        if (surface.GetWidth() > max_width && !current_line.empty()) {
            lines.push_back(current_line);
            current_line = word;
        } else {
            current_line = test_line;
        }
    }
    if (!current_line.empty()) {
        lines.push_back(current_line);
    }
    return lines;
}

static SDL_Rect calculate_visual_effect_dst(SDL2pp::Texture& texture, VisualEffectID effect_id,
                                            uint32_t pos_x, uint32_t pos_y, const SDL_Rect& camera,
                                            int offset_x, int offset_y) {
    int dst_w = texture.GetWidth();
    int dst_h = texture.GetHeight();

    // Si el efecto visual ocupa toda la camara
    if (dst_w >= camera.w && dst_h >= camera.h) {
        return SDL_Rect{offset_x, offset_y, camera.w, camera.h};
    }

    // Si el efecto visual pasa en una parte especifica del mapa
    SDL_Rect dst;
    dst.w = dst_w;
    dst.h = dst_h;
    dst.x = static_cast<int>(pos_x * TILE_SIZE) + (TILE_SIZE - dst.w) / 2 - camera.x + offset_x;
    if (effect_id == VisualEffectID::EXPLOSION) {
        dst.y = static_cast<int>(pos_y * TILE_SIZE) + (TILE_SIZE - dst.h) / 2 - camera.y + offset_y;
    } else {
        dst.y = static_cast<int>(pos_y * TILE_SIZE) + TILE_SIZE - dst.h - camera.y + offset_y;
    }
    return dst;
}

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
    camera.w = 672;  // Ancho lógico de visión del mapa (21 tiles)
    camera.h = 384;  // Alto lógico de visión del mapa (12 tiles)

    // Guardamos el offset físico de la pantalla donde se debe posicionar la cámara
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
    // Rectangulo vacio
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
                    // Cuanto ocupa el sprite
                    const int sprite_left = x * TILE_SIZE;
                    const int sprite_top =
                            (layer == Layer::Details || layer == Layer::Roof) ?
                                    y * TILE_SIZE :
                                    (y * TILE_SIZE + TILE_SIZE) - texture.GetHeight();
                    const int sprite_right = sprite_left + texture.GetWidth();
                    const int sprite_bottom = sprite_top + texture.GetHeight();
                    // Cuanto ocupa el rectangulo que contiene al sprite
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
    // La funcion para centrar la camara usa este rectangulo para saber hasta donde mostrar
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
        return;  // Si el jugador local no está en la escena, no movemos cámara

    // Obtenemos la posición interpolada del jugador
    const float player_x = it->second->get_pixel_x();
    const float player_y = it->second->get_pixel_y();

    // Centramos la cámara restando la mitad de sus dimensiones de visor
    const int target_x = static_cast<int>(player_x) + (TILE_SIZE / 2) - (camera.w / 2);
    const int target_y = static_cast<int>(player_y) + (TILE_SIZE / 2) - (camera.h / 2);

    // LIMITACIÓN DE BORDES: Evitamos que la cámara muestre negro fuera de los límites del mapa
    const int min_camera_x = visible_map_bounds.x;
    const int min_camera_y = visible_map_bounds.y;
    const int max_camera_x =
            std::max(min_camera_x, visible_map_bounds.x + visible_map_bounds.w - camera.w);
    const int max_camera_y =
            std::max(min_camera_y, visible_map_bounds.y + visible_map_bounds.h - camera.h);
    camera.x = std::clamp(target_x, min_camera_x, max_camera_x);
    camera.y = std::clamp(target_y, min_camera_y, max_camera_y);
}

void WorldRenderer::update_from_snapshot(const Snapshot& snapshot) {
    // Registro para saber qué entidades siguen activas en el rango de visión
    std::vector<uint32_t> ids_en_snapshot;
    // A. PROCESAR JUGADORES
    for (const auto& p_data: snapshot.players) {
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

    // B. PROCESAR NPCs (Criaturas / Ciudadanos)
    for (const auto& n_data: snapshot.npcs) {
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

    // C. PROCESAR ÍTEMS EN EL SUELO
    // TODO: Borrar estos prints de debug
    static size_t last_logged_count = 0;
    if (snapshot.items_on_floor.size() != last_logged_count) {
        last_logged_count = snapshot.items_on_floor.size();
        std::cerr << "[CLIENT] items_on_floor recibidos (primera vez): "
                  << snapshot.items_on_floor.size() << "\n";
        for (const auto& i: snapshot.items_on_floor) {
            std::cerr << "  item_id=" << static_cast<int>(i.item_id) << " pos=(" << i.position.x
                      << "," << i.position.y << ")\n";
        }
    }
    for (const auto& i_data: snapshot.items_on_floor) {
        // Generamos un ID único espacial (Spatial Hash).
        // Asumiendo que el mapa no mide más de 1000x1000 baldosas:
        // Ej: pos_x = 50, pos_y = 30 -> ID = 2000000 + 50000 + 30 = 2050030
        uint32_t item_client_id = item_entity_key(i_data.position.x, i_data.position.y);
        ids_en_snapshot.push_back(item_client_id);
        auto it = entities.find(item_client_id);
        if (it == entities.end()) {
            entities[item_client_id] = std::make_unique<RenderableEntity>(
                    item_client_id, EntityType::ITEM, i_data.position.x, i_data.position.y,
                    static_cast<uint8_t>(i_data.item_id), 0, 0, 0);
        }
    }

    // D. PROCESAR LIMPIEZA / CULLING (Desconexiones, muertes o salida de rango)
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

    const auto local_player = entities.find(player_entity_key(local_player_id));
    if (local_player != entities.end()) {
        const uint32_t player_x =
                static_cast<uint32_t>(local_player->second->get_pixel_x() / TILE_SIZE);
        const uint32_t player_y =
                static_cast<uint32_t>(local_player->second->get_pixel_y() / TILE_SIZE);
        const auto selected_sound_effects =
                select_audible_sound_effects(snapshot.sound_effects, player_x, player_y);
        for (const auto& sound_effect: selected_sound_effects) {
            SoundManager::play_effect(sound_effect.effect_id, sound_effect.pos_x,
                                      sound_effect.pos_y, player_x, player_y);
        }
    }

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
            // Si el efecto esta mal definido lo borro
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

// Renderizamos con el Algoritmo de pintor (Z-Order por eje Y)
void WorldRenderer::render() {
    if (!current_map)
        return;

    center_camera_on_player();

    SDL_RenderSetClipRect(renderer.Get(), nullptr);
    hud_renderer.render();

    SDL_Rect view_rect = {camera_screen_offset_x, camera_screen_offset_y, camera.w, camera.h};
    SDL_RenderSetClipRect(renderer.Get(), &view_rect);

    // Dibujamos algunos tiles extra alrededor de la cámara
    int start_tile_x = std::max(0, camera.x / TILE_SIZE - CULLING_MARGIN_TILES);
    int end_tile_x =
            std::min(current_map->width() - 1,
                     (camera.x + camera.w + TILE_SIZE - 1) / TILE_SIZE + CULLING_MARGIN_TILES);
    int start_tile_y = std::max(0, camera.y / TILE_SIZE - CULLING_MARGIN_TILES);
    int end_tile_y =
            std::min(current_map->height() - 1,
                     (camera.y + camera.h + TILE_SIZE - 1) / TILE_SIZE + CULLING_MARGIN_TILES);

    // 1. RENDERIZADO DEL SUELO (Background y Object)
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

                    int tex_w = texture.GetWidth();
                    int tex_h = texture.GetHeight();

                    SDL_Rect dst;
                    dst.x = (x * TILE_SIZE) - camera.x + camera_screen_offset_x;
                    dst.y = (y * TILE_SIZE + TILE_SIZE) - camera.y + camera_screen_offset_y - tex_h;
                    dst.w = tex_w;
                    dst.h = tex_h;

                    renderer.Copy(texture, SDL2pp::NullOpt, SDL2pp::Rect(dst));
                } catch (const std::exception& e) {
                    log_render_error_once(tex_key, e);
                }
            }
        }
    }

    // 2. RENDERIZADO DE ENTIDADES (Jugadores/NPCs)
    SDL_RenderSetClipRect(renderer.Get(), &view_rect);

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

        if (entity->get_type() != EntityType::PLAYER && entity->get_type() != EntityType::NPC &&
            entity->get_type() != EntityType::CITIZEN)
            continue;

        const int entity_screen_x = static_cast<int>(entity->get_pixel_x()) - camera.x +
                                    camera_screen_offset_x + TILE_SIZE / 2;
        const int entity_top_y =
                static_cast<int>(entity->get_pixel_y()) - camera.y + camera_screen_offset_y;

        int text_y = entity_top_y - 30;

        const bool is_local = (entity_key == player_entity_key(local_player_id));

        // Barra de vida para criaturas (NPCs)
        if (entity->get_type() == EntityType::NPC && entity->get_max_hp() > 0) {
            const uint16_t hp = entity->get_current_hp();
            const uint16_t max = entity->get_max_hp();
            const int bar_width = 60;
            const int bar_height = 6;
            const int bar_x = entity_screen_x - bar_width / 2;
            const int bar_y = text_y - bar_height - 4;

            // Borde (negro)
            renderer.SetDrawColor(0, 0, 0, 255);
            renderer.FillRect(SDL2pp::Rect(bar_x - 1, bar_y - 1, bar_width + 2, bar_height + 2));

            // Fondo (gris oscuro)
            renderer.SetDrawColor(50, 50, 50, 255);
            renderer.FillRect(SDL2pp::Rect(bar_x, bar_y, bar_width, bar_height));

            // Vida actual
            const int fill_width = static_cast<int>(bar_width * hp / max);
            if (hp > max / 2)
                renderer.SetDrawColor(50, 200, 50, 255);
            else if (hp > max / 4)
                renderer.SetDrawColor(200, 200, 50, 255);
            else
                renderer.SetDrawColor(200, 50, 50, 255);
            renderer.FillRect(SDL2pp::Rect(bar_x, bar_y, fill_width, bar_height));

            // Restaurar color a negro para no afectar el HUD
            renderer.SetDrawColor(0, 0, 0, 255);

            text_y = bar_y - 4;
        }

        // Nivel (solo jugadores)
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

        // Nombre
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
                name_color = {255, 100, 100, 255};  // Rojo para criaturas
            } else if (entity->get_type() == EntityType::CITIZEN) {
                name_color = {100, 200, 255, 255};  // Azul para ciudadanos
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

        // Burbuja de chat (solo jugadores)
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

    const RenderableEntity* local_player = nullptr;
    const auto local_player_it = entities.find(player_entity_key(local_player_id));
    if (local_player_it != entities.end()) {
        local_player = local_player_it->second.get();
    }

    // 3. RENDERIZADO DE DETAILS DELANTE DE LAS ENTIDADES
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

    // 4. RENDERIZADO DE TECHOS (Roofs)
    SDL_RenderSetClipRect(renderer.Get(), &view_rect);

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
