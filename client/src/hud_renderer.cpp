#include "../includes/hud_renderer.h"

HudRenderer::HudRenderer(SDL2pp::Renderer& r, TextureManager& tm, int width, int height)
    : renderer(r), texture_manager(tm), w_width(width), w_height(height) {}

void HudRenderer::render() const {
    // 1. Dibujar el Layer Superior
    SDL2pp::Rect upperLayerRect(0, 0, w_width, w_height);
    renderer.Copy(texture_manager.get_texture("hud_upper_layer"),
                  SDL2pp::NullOpt,
                  upperLayerRect);

    // 2. Base del Inventario
    SDL2pp::Rect inventoryRect(w_width - PANEL_RIGHT_OFFSET, INVENTORY_Y, INVENTORY_WIDTH, INVENTORY_HEIGHT);
    renderer.Copy(texture_manager.get_texture("hud_inventory_base"),
                  SDL2pp::NullOpt,
                  inventoryRect);

    // 3. Base de Estadísticas
    SDL2pp::Rect statsRect(w_width - PANEL_RIGHT_OFFSET, STATS_Y, STATS_WIDTH, STATS_HEIGHT);
    renderer.Copy(texture_manager.get_texture("hud_stats_base"),
                  SDL2pp::NullOpt,
                  statsRect);

    // 4. Base de la Consola
    SDL2pp::Rect consoleRect(0, w_height - CONSOLE_HEIGHT, w_width - PANEL_RIGHT_OFFSET, CONSOLE_HEIGHT);
    renderer.Copy(texture_manager.get_texture("hud_console_base"),
                  SDL2pp::NullOpt,
                  consoleRect);
}