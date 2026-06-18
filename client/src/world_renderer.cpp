#include "client/includes/world_renderer.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>
#include <unordered_set>

#include "common/includes/types.h"

static constexpr int MAX_TILE_TEXTURE_SIZE = 1024;
static constexpr int CULLING_MARGIN_TILES = MAX_TILE_TEXTURE_SIZE / TILE_SIZE + 2;
static constexpr uint32_t PLAYER_ENTITY_OFFSET = 0;
static constexpr uint32_t NPC_ENTITY_OFFSET = 1000000;
static constexpr uint32_t ITEM_ENTITY_OFFSET = 2000000;

static uint32_t player_entity_key(uint32_t server_id) { return PLAYER_ENTITY_OFFSET + server_id; }

static uint32_t npc_entity_key(uint32_t server_id) { return NPC_ENTITY_OFFSET + server_id; }

static uint32_t item_entity_key(uint32_t pos_x, uint32_t pos_y) {
    return ITEM_ENTITY_OFFSET + (pos_x * 1000) + pos_y;
}

static uint32_t exp_next_level(uint8_t level) {
    return static_cast<uint32_t>(1000 * std::pow(level, 1.8));
}

static void log_render_error_once(const std::string& texture_key, const std::exception& e) {
    static std::unordered_set<std::string> logged_errors;
    if (logged_errors.insert(texture_key).second) {
        std::cerr << "[WorldRenderer] No se pudo renderizar " << texture_key << ": " << e.what()
                  << std::endl;
    }
}

WorldRenderer::WorldRenderer(SDL2pp::Renderer& renderer_, TextureManager& texture_manager_):
        renderer(renderer_),
        texture_manager(texture_manager_),
        hud_renderer(renderer, texture_manager, WINDOW_W, WINDOW_H),
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

void WorldRenderer::set_player_name(const std::string& name) { hud_renderer.set_player_name(name); }

void WorldRenderer::update_hud_stats(const MsgPlayerStats& stats) {
    hud_renderer.update_stats(stats);
}

void WorldRenderer::load_map(Map&& new_map) {
    entities.clear();
    current_map = std::move(new_map);
    camera.x = 0;
    camera.y = 0;
    update_visible_map_bounds();
    /*std::cout << "[WorldRenderer] Nuevo mapa binario inyectado correctamente de la red. Dimensión:
       "
              << current_map->width() << "x" << current_map->height() << " tiles." << std::endl;*/
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
                    const int sprite_top = (y * TILE_SIZE + TILE_SIZE) - texture.GetHeight();
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

void WorldRenderer::add_chat_message(const std::string& msg) { hud_renderer.add_chat_message(msg); }

void WorldRenderer::update_chat_input(const std::string& buffer, bool is_active) {
    hud_renderer.update_chat_input(buffer, is_active);
}

bool WorldRenderer::is_point_inside_console(const uint32_t x, const uint32_t y) const {
    return hud_renderer.is_point_inside_console(x, y);
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
            MsgPlayerStats stats;
            stats.hp = p_data.stats.current_hp;
            stats.max_hp = p_data.stats.max_hp;
            stats.mana = p_data.stats.current_mana;
            stats.max_mana = p_data.stats.max_mana;
            stats.exp = p_data.stats.xp;
            stats.exp_next_level = exp_next_level(p_data.stats.level);
            stats.level = p_data.stats.level;
            hud_renderer.update_stats(stats);
        }
        auto it = entities.find(entity_key);
        if (it != entities.end()) {
            it->second->move_to(p_data.pos_x, p_data.pos_y,
                                static_cast<Direction>(p_data.direction));
        } else {
            bool is_short = (p_data.ch_traits.race == GNOME || p_data.ch_traits.race == DWARF);
            entities[entity_key] = std::make_unique<RenderableEntity>(
                    entity_key, EntityType::PLAYER, p_data.pos_x, p_data.pos_y,
                    p_data.ch_traits.body, p_data.ch_traits.head, p_data.weapon_id,
                    p_data.shield_id, is_short);
        }
    }

    // B. PROCESAR NPCs (Criaturas / Ciudadanos)
    for (const auto& n_data: snapshot.npcs) {
        const uint32_t entity_key = npc_entity_key(n_data.id);
        ids_en_snapshot.push_back(entity_key);
        auto it = entities.find(entity_key);
        if (it != entities.end()) {
            it->second->move_to(n_data.pos_x, n_data.pos_y,
                                DOWN);  // Ojo: cuando tengas dirección de NPC, ponla aquí
        } else {
            entities[entity_key] = std::make_unique<RenderableEntity>(
                    entity_key, EntityType::NPC, n_data.pos_x, n_data.pos_y,
                    static_cast<uint8_t>(n_data.type_id), 0, 0, 0);
        }
    }

    // C. PROCESAR ÍTEMS EN EL SUELO
    for (const auto& i_data: snapshot.items_on_floor) {
        // Generamos un ID único espacial (Spatial Hash).
        // Asumiendo que el mapa no mide más de 1000x1000 baldosas:
        // Ej: pos_x = 50, pos_y = 30 -> ID = 2000000 + 50000 + 30 = 2050030
        uint32_t item_client_id = item_entity_key(i_data.pos_x, i_data.pos_y);
        ids_en_snapshot.push_back(item_client_id);
        auto it = entities.find(item_client_id);
        if (it == entities.end()) {
            entities[item_client_id] = std::make_unique<RenderableEntity>(
                    item_client_id, EntityType::ITEM, i_data.pos_x, i_data.pos_y,
                    static_cast<uint8_t>(i_data.item_id), 0, 0, 0);
        }
    }

    // D. PROCESAR LIMPIEZA / CULLING (Desconexiones, muertes o salida de rango)
    for (auto it = entities.begin(); it != entities.end();) {
        uint32_t entity_id = it->first;

        const bool sigue_viva = (std::find(ids_en_snapshot.begin(), ids_en_snapshot.end(),
                                           entity_id) != ids_en_snapshot.end());

        if (!sigue_viva) {
            it = entities.erase(it);
        } else {
            ++it;
        }
    }
}

void WorldRenderer::update_animations(float dt) {
    for (auto& [id, entity]: entities) {
        entity->update(dt);
    }
}

// Renderizamos con el Algoritmo de pintor (Z-Order por eje Y)
void WorldRenderer::render() {
    // HARCODEADO! centra la camara
    // camera.x = 0;
    // camera.y = 0;

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

    // 1. RENDERIZADO DEL SUELO (Background, Details y Object)
    std::array<std::pair<Layer, std::string>, 3> ground_layers = {{
            {Layer::Background, "tile_bg_"},
            {Layer::Details, "tile_det_"},
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

    std::vector<RenderableEntity*> sorted_entities;
    sorted_entities.reserve(entities.size());
    for (const auto& [id, entity]: entities) {
        sorted_entities.push_back(entity.get());
    }

    std::sort(sorted_entities.begin(), sorted_entities.end(),
              [](const RenderableEntity* a, const RenderableEntity* b) {
                  if (a->get_pixel_y() != b->get_pixel_y())
                      return a->get_pixel_y() < b->get_pixel_y();
                  return a->get_id() < b->get_id();
              });

    for (auto* entity: sorted_entities) {
        // Le pasamos la posición de la cámara y los offsets de pantalla al método render de la
        // entidad
        entity->render_with_camera(renderer, texture_manager, camera.x, camera.y,
                                   camera_screen_offset_x, camera_screen_offset_y);
    }

    // 3. RENDERIZADO DE TECHOS (Roofs)
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
    SDL_RenderSetClipRect(renderer.Get(), nullptr);
}

std::optional<std::pair<uint32_t, EntityType>> WorldRenderer::get_entity_at_screen(
        int screen_x, int screen_y) const {
    for (const auto& [id, entity]: entities) {
        if (entity->get_type() == EntityType::ITEM)
            continue;
        if (id == local_player_id)
            continue;
        int ex = static_cast<int>(entity->get_pixel_x()) - camera.x + camera_screen_offset_x;
        int ey = static_cast<int>(entity->get_pixel_y()) - camera.y + camera_screen_offset_y;
        if (screen_x >= ex && screen_x < ex + TILE_SIZE && screen_y >= ey &&
            screen_y < ey + TILE_SIZE) {
            return std::make_pair(id, entity->get_type());
        }
    }
    return std::nullopt;
}
