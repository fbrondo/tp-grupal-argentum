#include "client/includes/world_renderer.h"

#include <algorithm>
#include <iostream>

#include "common/includes/types.h"

WorldRenderer::WorldRenderer(SDL2pp::Renderer& renderer_, TextureManager& texture_manager_):
        renderer(renderer_),
        texture_manager(texture_manager_),
        hud_renderer(renderer, texture_manager, WINDOW_W, WINDOW_H),
        local_player_id(0),
        current_map(std::nullopt) {

    camera.x = 0;
    camera.y = 0;
    camera.w = 672;  // Ancho lógico de visión del mapa (21 tiles)
    camera.h = 384;  // Alto lógico de visión del mapa (12 tiles)

    // Guardamos el offset físico de la pantalla donde se debe posicionar la cámara
    camera_screen_offset_x = 7;
    camera_screen_offset_y = 149;
}

void WorldRenderer::set_local_player(const uint32_t id) { local_player_id = id; }

void WorldRenderer::load_map(Map&& new_map) {
    entities.clear();
    current_map = std::move(new_map);
    camera.x = 0;
    camera.y = 0;
    /*std::cout << "[WorldRenderer] Nuevo mapa binario inyectado correctamente de la red. Dimensión: "
              << current_map->width() << "x" << current_map->height() << " tiles." << std::endl;*/
}

void WorldRenderer::center_camera_on_player() {
    if (!current_map)
        return;

    const auto it = entities.find(local_player_id);
    if (it == entities.end())
        return;  // Si el jugador local no está en la escena, no movemos cámara

    // Obtenemos la posición interpolada del jugador
    const float player_x = it->second->get_pixel_x();
    const float player_y = it->second->get_pixel_y();

    // Centramos la cámara restando la mitad de sus dimensiones de visor
    camera.x = static_cast<int>(player_x) - (camera.w / 2);
    camera.y = static_cast<int>(player_y) - (camera.h / 2);

    // LIMITACIÓN DE BORDES: Evitamos que la cámara muestre negro fuera de los límites del mapa
    const int max_width_px = current_map->width() * TILE_SIZE;
    const int max_height_px = current_map->height() * TILE_SIZE;

    if (camera.x < 0)
        camera.x = 0;
    if (camera.y < 0)
        camera.y = 0;
    if (camera.x > max_width_px - camera.w)
        camera.x = max_width_px - camera.w;
    if (camera.y > max_height_px - camera.h)
        camera.y = max_height_px - camera.h;
}

void WorldRenderer::update_from_snapshot(const Snapshot& snapshot) {
    // Registro para saber qué entidades siguen activas en el rango de visión
    std::vector<uint32_t> ids_en_snapshot;
    // A. PROCESAR JUGADORES
    for (const auto& p_data: snapshot.players) {
        ids_en_snapshot.push_back(p_data.id);
        auto it = entities.find(p_data.id);
        if (it != entities.end()) {
            it->second->move_to(p_data.pos_x, p_data.pos_y,
                                static_cast<Direction>(p_data.direction));
        } else {
            bool is_short = (p_data.raza == GNOME || p_data.raza == DWARF);
            entities[p_data.id] = std::make_unique<RenderableEntity>(
                    p_data.id, EntityType::PLAYER, p_data.pos_x, p_data.pos_y, p_data.body_id,
                    p_data.head_id, p_data.weapon_id, p_data.shield_id, is_short);
            entities[p_data.id]->move_to(p_data.pos_x, p_data.pos_y,
                                         static_cast<Direction>(p_data.direction));
        }
    }

    // B. PROCESAR NPCs (Criaturas / Ciudadanos)
    for (const auto& n_data: snapshot.npcs) {
        ids_en_snapshot.push_back(n_data.id);
        auto it = entities.find(n_data.id);
        if (it != entities.end()) {
            it->second->move_to(n_data.pos_x, n_data.pos_y,
                                DOWN);  // Ojo: cuando tengas dirección de NPC, ponla aquí
        } else {
            entities[n_data.id] = std::make_unique<RenderableEntity>(
                    n_data.id, EntityType::NPC, n_data.pos_x, n_data.pos_y,
                    static_cast<uint8_t>(n_data.type_id), 0, 0, 0);
            entities[n_data.id]->move_to(n_data.pos_x, n_data.pos_y, DOWN);
        }
    }

    // C. PROCESAR ÍTEMS EN EL SUELO
    for (const auto& i_data: snapshot.items_on_floor) {
        // Generamos un ID único espacial (Spatial Hash).
        // Asumiendo que el mapa no mide más de 1000x1000 baldosas:
        // Ej: pos_x = 50, pos_y = 30 -> ID = 2000000 + 50000 + 30 = 2050030
        uint32_t item_client_id = 2000000 + (i_data.pos_x * 1000) + i_data.pos_y;
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

    // Matemática de Tile Culling (Se mantiene igual, adaptada al tamaño real de la cámara)
    int start_tile_x = std::max(0, camera.x / TILE_SIZE);
    int end_tile_x = std::min(current_map->width() - 1, (camera.x + camera.w) / TILE_SIZE + 1);
    int start_tile_y = std::max(0, camera.y / TILE_SIZE);
    int end_tile_y = std::min(current_map->height() - 1, (camera.y + camera.h) / TILE_SIZE + 1);

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

                try {
                    std::string tex_key = key_prefix + std::to_string(sprite_id);
                    SDL2pp::Texture& texture = texture_manager.get_texture(tex_key);

                    int tex_w = texture.GetWidth();
                    int tex_h = texture.GetHeight();

                    SDL_Rect dst;
                    dst.x = (x * TILE_SIZE) - camera.x + camera_screen_offset_x;
                    dst.y = (y * TILE_SIZE + TILE_SIZE) - camera.y + camera_screen_offset_y - tex_h;
                    dst.w = tex_w;
                    dst.h = tex_h;

                    renderer.Copy(texture, SDL2pp::NullOpt, SDL2pp::Rect(dst));
                } catch (...) {}
            }
        }
    }

    // 2. RENDERIZADO DE ENTIDADES (Jugadores/NPCs)
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
    for (int y = start_tile_y; y <= end_tile_y; ++y) {
        for (int x = start_tile_x; x <= end_tile_x; ++x) {
            auto& tile_data = current_map->tile_at(x, y, Layer::Roof);
            int32_t sprite_id = tile_data.sprite_id;

            if (sprite_id == 0)
                continue;

            try {
                std::string tex_key = "tile_roof_" + std::to_string(sprite_id);
                SDL2pp::Texture& texture = texture_manager.get_texture(tex_key);

                int tex_w = texture.GetWidth();
                int tex_h = texture.GetHeight();

                SDL_Rect dst;
                dst.x = (x * TILE_SIZE) - camera.x + camera_screen_offset_x;
                dst.y = (y * TILE_SIZE + TILE_SIZE) - camera.y + camera_screen_offset_y - tex_h;
                dst.w = tex_w;
                dst.h = tex_h;

                renderer.Copy(texture, SDL2pp::NullOpt, SDL2pp::Rect(dst));
            } catch (...) {}
        }
    }
    SDL_RenderSetClipRect(renderer.Get(), nullptr);
}
