#include "../includes/hud_renderer.h"

HudRenderer::HudRenderer(SDL2pp::Renderer& r, TextureManager& tm, int width, int height)
    : renderer(r), texture_manager(tm), w_width(width), w_height(height) {}

void HudRenderer::render() const {
    // Upper layer (full screen overlay)
    renderer.Copy(texture_manager.get_texture("hud_upper_layer"),
                  SDL2pp::NullOpt,
                  SDL2pp::Rect(0, 0, w_width, w_height));

    // Consola (arriba izquierda)
    renderer.Copy(texture_manager.get_texture("hud_console_base"),
                  SDL2pp::NullOpt,
                  SDL2pp::Rect(CONSOLE_X, CONSOLE_Y, CONSOLE_W, CONSOLE_H));

    // User info (arriba derecha)
    renderer.Copy(texture_manager.get_texture("hud_user_info_base"),
                  SDL2pp::NullOpt,
                  SDL2pp::Rect(PANEL_RIGHT_X, USER_INFO_Y, PANEL_RIGHT_W, USER_INFO_H));

    // Inventario
    renderer.Copy(texture_manager.get_texture("hud_inventory_base"),
                  SDL2pp::NullOpt,
                  SDL2pp::Rect(PANEL_RIGHT_X, INVENTORY_Y, INVENTORY_W, INVENTORY_H));

    // Stats (vida/mana)
    renderer.Copy(texture_manager.get_texture("hud_stats_base"),
                  SDL2pp::NullOpt,
                  SDL2pp::Rect(PANEL_RIGHT_X, STATS_Y, STATS_W, STATS_H));
}
