#include "../includes/hud_renderer.h"

#include <algorithm>
#include <string>

#include "client/includes/chat_manager.h"

namespace {
constexpr int INVENTORY_SLOT_SIZE = 32;
constexpr int INVENTORY_SLOT_GAP_X = 7;
constexpr int INVENTORY_SLOT_GAP_Y = 8;
constexpr int INVENTORY_SLOT_COLUMNS = 6;
constexpr int INVENTORY_GRID_X = PANEL_RIGHT_X + 20;
constexpr int INVENTORY_GRID_Y = INVENTORY_Y + 122;
constexpr int EQUIPMENT_SLOT_SIZE = 32;
constexpr int EQUIPMENT_GRID_Y = INVENTORY_Y + 40;
constexpr int EQUIPMENT_WEAPON_X = PANEL_RIGHT_X + 36;
constexpr int EQUIPMENT_HELMET_X = PANEL_RIGHT_X + 112;
constexpr int EQUIPMENT_ARMOR_X = PANEL_RIGHT_X + 156;
constexpr int EQUIPMENT_SHIELD_X = PANEL_RIGHT_X + 205;

std::optional<SDL_Rect> equipment_rect_for_item(uint8_t type_item) {
    switch (static_cast<TypeItem>(type_item)) {
        case SWORD:
        case AXE:
        case HAMMER:
        case ASH_STAFF:
        case ELVEN_FLUTE:
        case KNOTTED_STAFF:
        case INLAID_STAFF:
        case SIMPLE_BOW:
        case COMPOUND_BOW:
            return SDL_Rect{EQUIPMENT_WEAPON_X, EQUIPMENT_GRID_Y, EQUIPMENT_SLOT_SIZE,
                            EQUIPMENT_SLOT_SIZE};
        case HOOD:
        case IRON_HELMET:
        case MAGIC_HAT:
            return SDL_Rect{EQUIPMENT_HELMET_X, EQUIPMENT_GRID_Y, EQUIPMENT_SLOT_SIZE,
                            EQUIPMENT_SLOT_SIZE};
        case LEATHER_ARMOR:
        case PLATE_AMOR:
        case BLUE_TUNIC:
            return SDL_Rect{EQUIPMENT_ARMOR_X, EQUIPMENT_GRID_Y, EQUIPMENT_SLOT_SIZE,
                            EQUIPMENT_SLOT_SIZE};
        case TORTOISE_SHIELD:
        case IRON_SHIELD:
            return SDL_Rect{EQUIPMENT_SHIELD_X, EQUIPMENT_GRID_Y, EQUIPMENT_SLOT_SIZE,
                            EQUIPMENT_SLOT_SIZE};
        default:
            return std::nullopt;
    }
}
}  // namespace


HudRenderer::HudRenderer(SDL2pp::Renderer& r, TextureManager& tm, FontManager& fm, int width,
                         int height):
        renderer(r), texture_manager(tm), fonts(fm), w_width(width), w_height(height) {}

std::unique_ptr<SDL2pp::Texture> HudRenderer::create_text_texture(const std::string& text) {
    if (text.empty()) {
        return nullptr;
    }
    return std::make_unique<SDL2pp::Texture>(
            renderer,
            fonts.get_console_font().RenderUTF8_Blended(text, SDL_Color{255, 255, 255, 255}));
}

std::unique_ptr<SDL2pp::Texture> HudRenderer::create_input_texture(const std::string& text) {
    if (text.empty()) {
        return nullptr;
    }
    return std::make_unique<SDL2pp::Texture>(
            renderer,
            fonts.get_console_input_font().RenderUTF8_Blended(text, SDL_Color{255, 255, 255, 255}));
}

std::unique_ptr<SDL2pp::Texture> HudRenderer::create_hud_texture(const std::string& text) {
    if (text.empty()) {
        return nullptr;
    }
    return std::make_unique<SDL2pp::Texture>(
            renderer, fonts.get_hud_font().RenderUTF8_Blended(text, SDL_Color{255, 255, 255, 255}));
}

std::unique_ptr<SDL2pp::Texture> HudRenderer::create_text_texture_colored(const std::string& text,
                                                                          SDL_Color color) {
    if (text.empty()) {
        return nullptr;
    }
    return std::make_unique<SDL2pp::Texture>(
            renderer, fonts.get_console_font().RenderUTF8_Blended(text, color));
}

void HudRenderer::render_centered_text(const std::unique_ptr<SDL2pp::Texture>& texture, int x,
                                       int y, int width, int height) const {
    if (!texture) {
        return;
    }

    int render_width = texture->GetWidth();
    int render_height = texture->GetHeight();
    const int max_width = width - 4;
    if (render_width > max_width) {
        render_height = render_height * max_width / render_width;
        render_width = max_width;
    }

    renderer.Copy(*texture, SDL2pp::NullOpt,
                  SDL2pp::Rect(x + (width - render_width) / 2, y + (height - render_height) / 2,
                               render_width, render_height));
}

void HudRenderer::update_player_textures() {
    player_name_texture = create_hud_texture(player_name);
}

void HudRenderer::update_stats_textures() {
    level_texture = create_hud_texture(std::to_string(stats.level));
    hp_texture = create_text_texture(std::to_string(stats.hp) + "/" + std::to_string(stats.max_hp));
    mana_texture =
            create_text_texture(std::to_string(stats.mana) + "/" + std::to_string(stats.max_mana));
    exp_texture = create_text_texture(std::to_string(stats.exp) + "/" +
                                      std::to_string(stats.exp_next_level));
}

void HudRenderer::set_player_name(const std::string& name) {
    player_name = name;
    update_player_textures();
}

void HudRenderer::update_stats(const MsgPlayerStats& new_stats) {
    const bool values_changed =
            stats.hp != new_stats.hp || stats.max_hp != new_stats.max_hp ||
            stats.mana != new_stats.mana || stats.max_mana != new_stats.max_mana ||
            stats.exp != new_stats.exp || stats.exp_next_level != new_stats.exp_next_level ||
            stats.level != new_stats.level;
    stats = new_stats;
    if (values_changed) {
        update_stats_textures();
    }
}

void HudRenderer::update_inventory(const std::vector<MsgSlot>& slots) {
    inventory_slots.clear();
    for (const auto& slot: slots) {
        if (slot.type_item == NONE || slot.quantity == 0) {
            continue;
        }
        inventory_slots[slot.slot_index] = slot;
    }
}

void HudRenderer::update_equipment(const std::vector<MsgSlot>& slots) {
    equipped_inventory_slots.clear();
    for (const auto& slot: slots) {
        if (slot.type_item == NONE || slot.quantity == 0) {
            continue;
        }
        equipped_inventory_slots[slot.slot_index] = slot;
    }
}

float calculate_percentage(uint32_t current, uint32_t maximum) {
    if (maximum == 0)
        return 0.0f;

    float ratio = static_cast<float>(current) / static_cast<float>(maximum);
    return std::clamp(ratio, 0.0f, 1.0f);
}

void HudRenderer::render_progress_bar(int x, int y, int width, int height, uint32_t current,
                                      uint32_t maximum, SDL_Color color) const {
    SDL_Rect background = {x, y, width, height};
    SDL_SetRenderDrawColor(renderer.Get(), 0, 0, 0, 255);
    SDL_RenderFillRect(renderer.Get(), &background);

    const float ratio = calculate_percentage(current, maximum);

    SDL_Rect fill = {x + 2, y + 2, static_cast<int>((width - 4) * ratio), height - 4};
    SDL_SetRenderDrawColor(renderer.Get(), color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer.Get(), &fill);

    renderer.Copy(texture_manager.get_texture("hud_pb_bisel"), SDL2pp::NullOpt,
                  SDL2pp::Rect(x, y, width, height));
}

bool HudRenderer::is_point_inside_console(const uint32_t x, const uint32_t y) const {
    return (x >= CONSOLE_X && x <= CONSOLE_X + CONSOLE_W && y >= CONSOLE_Y &&
            y <= CONSOLE_Y + CONSOLE_H);
}

bool HudRenderer::is_point_inside_console_input(uint32_t x, uint32_t y) const {
    return (x >= CONSOLE_X && x <= CONSOLE_X + CONSOLE_W && y >= CONSOLE_INPUT_Y &&
            y <= CONSOLE_Y + CONSOLE_H);
}


std::optional<uint8_t> HudRenderer::inventory_slot_at(const uint32_t x, const uint32_t y) const {
    if (x < INVENTORY_GRID_X || y < INVENTORY_GRID_Y) {
        return std::nullopt;
    }

    const uint32_t local_x = x - INVENTORY_GRID_X;
    const uint32_t local_y = y - INVENTORY_GRID_Y;
    const uint32_t cell_w = INVENTORY_SLOT_SIZE + INVENTORY_SLOT_GAP_X;
    const uint32_t cell_h = INVENTORY_SLOT_SIZE + INVENTORY_SLOT_GAP_Y;
    const uint32_t col = local_x / cell_w;
    const uint32_t row = local_y / cell_h;

    if (col >= INVENTORY_SLOT_COLUMNS || local_x % cell_w >= INVENTORY_SLOT_SIZE ||
        local_y % cell_h >= INVENTORY_SLOT_SIZE) {
        return std::nullopt;
    }

    const auto slot_index = static_cast<uint8_t>(row * INVENTORY_SLOT_COLUMNS + col);
    if (!inventory_slots.contains(slot_index) || equipped_inventory_slots.contains(slot_index)) {
        return std::nullopt;
    }
    return slot_index;
}

std::optional<uint8_t> HudRenderer::equipment_slot_at(const uint32_t x, const uint32_t y) const {
    for (const auto& [inventory_slot, slot]: equipped_inventory_slots) {
        const auto rect = equipment_rect_for_item(slot.type_item);
        if (!rect.has_value()) {
            continue;
        }
        if (x >= static_cast<uint32_t>(rect->x) && x < static_cast<uint32_t>(rect->x + rect->w) &&
            y >= static_cast<uint32_t>(rect->y) && y < static_cast<uint32_t>(rect->y + rect->h)) {
            return inventory_slot;
        }
    }
    return std::nullopt;
}

void HudRenderer::add_chat_message(const std::string& msg, MessageColor color) {
    if (msg.empty())
        return;

    SDL_Color sdl_color;
    switch (color) {
        case COLOR_GREEN:
            sdl_color = {140, 220, 140, 255};
            break;
        case COLOR_RED:
            sdl_color = {220, 80, 80, 255};
            break;
        case COLOR_YELLOW:
            sdl_color = {210, 170, 45, 255};
            break;
        case COLOR_BLUE:
            sdl_color = {100, 150, 255, 255};
            break;
        default:
            sdl_color = {255, 255, 255, 255};
            break;
    }
    chat_log_textures.push_back(create_text_texture_colored(msg, sdl_color));

    if (chat_log_textures.size() > MAX_CHAT_LOG_SIZE) {
        chat_log_textures.pop_front();
    }

    constexpr int LINE_SPACING = 18;
    int messages_area_h = CONSOLE_H - CONSOLE_INPUT_H - 5;
    int total_lines = static_cast<int>(chat_log_textures.size());
    int max_visible = messages_area_h / LINE_SPACING;
    console_scroll_offset = std::max(0, total_lines - max_visible);
}

void HudRenderer::update_chat_input(const std::string& buffer, bool is_active) {
    chat_is_active = is_active;
    if (is_active) {
        std::string prompt = "> " + buffer + "_";
        chat_input_texture = create_input_texture(prompt);
    } else {
        chat_input_texture = nullptr;
    }
}

void HudRenderer::update_resurrection_timer(uint16_t time_left_ms) {
    if (resurrection_time_left_ms == time_left_ms) {
        return;
    }
    resurrection_time_left_ms = time_left_ms;
    if (resurrection_time_left_ms == 0) {
        resurrection_texture = nullptr;
        return;
    }

    const auto seconds_left = static_cast<uint16_t>((resurrection_time_left_ms + 999) / 1000);
    resurrection_texture =
            create_text_texture("Estas resucitando... " + std::to_string(seconds_left) + "s");
}

void HudRenderer::render_resurrection_notice() const {
    if (!resurrection_texture) {
        return;
    }

    constexpr int NOTICE_W = 300;
    constexpr int NOTICE_H = 48;
    constexpr int NOTICE_X = CONSOLE_X + (CONSOLE_W - NOTICE_W) / 2;
    constexpr int NOTICE_Y = 170;

    SDL_Rect background = {NOTICE_X, NOTICE_Y, NOTICE_W, NOTICE_H};
    SDL_SetRenderDrawColor(renderer.Get(), 12, 12, 16, 220);
    SDL_RenderFillRect(renderer.Get(), &background);
    SDL_SetRenderDrawColor(renderer.Get(), 210, 190, 120, 255);
    SDL_RenderDrawRect(renderer.Get(), &background);
    render_centered_text(resurrection_texture, NOTICE_X, NOTICE_Y, NOTICE_W, NOTICE_H);
    SDL_SetRenderDrawColor(renderer.Get(), 0, 0, 0, 255);
}

void HudRenderer::render_chat() const {
    constexpr int LINE_SPACING = 18;
    int messages_area_h = CONSOLE_H - CONSOLE_INPUT_H - 5;

    int total_lines = static_cast<int>(chat_log_textures.size());
    int max_visible = messages_area_h / LINE_SPACING;
    int max_scroll = std::max(0, total_lines - max_visible);

    int scroll = std::min(console_scroll_offset, max_scroll);
    scroll = std::max(scroll, 0);

    SDL_Rect clip_rect = {CONSOLE_X, CONSOLE_Y, CONSOLE_W, messages_area_h + 5};
    SDL_RenderSetClipRect(renderer.Get(), &clip_rect);

    int start_y = CONSOLE_Y + 5;
    for (int i = scroll; i < total_lines; ++i) {
        const auto& tex = chat_log_textures[i];
        if (tex) {
            renderer.Copy(*tex, SDL2pp::NullOpt,
                          SDL2pp::Rect(CONSOLE_X + 10, start_y, tex->GetWidth(), tex->GetHeight()));
            start_y += LINE_SPACING;
            if (start_y + LINE_SPACING > CONSOLE_INPUT_Y)
                break;
        }
    }

    SDL_RenderSetClipRect(renderer.Get(), nullptr);

    SDL_SetRenderDrawColor(renderer.Get(), 80, 80, 80, 255);
    SDL_RenderDrawLine(renderer.Get(), CONSOLE_X, CONSOLE_INPUT_Y, CONSOLE_X + CONSOLE_W,
                       CONSOLE_INPUT_Y);

    if (chat_is_active && chat_input_texture) {
        renderer.Copy(
                *chat_input_texture, SDL2pp::NullOpt,
                SDL2pp::Rect(CONSOLE_X + 10, CONSOLE_INPUT_Y + 3, chat_input_texture->GetWidth(),
                             chat_input_texture->GetHeight()));
    }
}

void HudRenderer::render_inventory() const {
    for (const auto& [slot_index, slot]: inventory_slots) {
        if (equipped_inventory_slots.contains(slot_index)) {
            continue;
        }

        const int col = slot_index % INVENTORY_SLOT_COLUMNS;
        const int row = slot_index / INVENTORY_SLOT_COLUMNS;
        const int x = INVENTORY_GRID_X + col * (INVENTORY_SLOT_SIZE + INVENTORY_SLOT_GAP_X);
        const int y = INVENTORY_GRID_Y + row * (INVENTORY_SLOT_SIZE + INVENTORY_SLOT_GAP_Y);

        try {
            SDL2pp::Texture& item_texture =
                    texture_manager.get_texture("item_" + std::to_string(slot.type_item));
            const int tex_w = item_texture.GetWidth();
            const int tex_h = item_texture.GetHeight();
            const int scale = std::max(tex_w, tex_h);
            int dst_w = tex_w;
            int dst_h = tex_h;
            if (scale > INVENTORY_SLOT_SIZE) {
                dst_w = tex_w * INVENTORY_SLOT_SIZE / scale;
                dst_h = tex_h * INVENTORY_SLOT_SIZE / scale;
            }
            renderer.Copy(item_texture, SDL2pp::NullOpt,
                          SDL2pp::Rect(x + (INVENTORY_SLOT_SIZE - dst_w) / 2,
                                       y + (INVENTORY_SLOT_SIZE - dst_h) / 2, dst_w, dst_h));
        } catch (...) {}
    }

    for (const auto& [slot_index, slot]: equipped_inventory_slots) {
        const auto rect = equipment_rect_for_item(slot.type_item);
        if (!rect.has_value()) {
            continue;
        }

        try {
            renderer.Copy(texture_manager.get_texture("hud_inventory_selected"), SDL2pp::NullOpt,
                          SDL2pp::Rect(*rect));
            SDL2pp::Texture& item_texture =
                    texture_manager.get_texture("item_" + std::to_string(slot.type_item));
            const int tex_w = item_texture.GetWidth();
            const int tex_h = item_texture.GetHeight();
            const int scale = std::max(tex_w, tex_h);
            int dst_w = tex_w;
            int dst_h = tex_h;
            if (scale > EQUIPMENT_SLOT_SIZE) {
                dst_w = tex_w * EQUIPMENT_SLOT_SIZE / scale;
                dst_h = tex_h * EQUIPMENT_SLOT_SIZE / scale;
            }
            renderer.Copy(item_texture, SDL2pp::NullOpt,
                          SDL2pp::Rect(rect->x + (EQUIPMENT_SLOT_SIZE - dst_w) / 2,
                                       rect->y + (EQUIPMENT_SLOT_SIZE - dst_h) / 2, dst_w, dst_h));
        } catch (...) {}
    }
}

void HudRenderer::scroll_console(int delta) {
    if (chat_log_textures.empty())
        return;

    constexpr int LINE_SPACING = 18;
    int messages_area_h = CONSOLE_H - CONSOLE_INPUT_H - 5;
    int total_lines = static_cast<int>(chat_log_textures.size());
    int max_visible = messages_area_h / LINE_SPACING;
    int max_scroll = std::max(0, total_lines - max_visible);

    console_scroll_offset += delta;
    if (console_scroll_offset < 0)
        console_scroll_offset = 0;
    if (console_scroll_offset > max_scroll)
        console_scroll_offset = max_scroll;
}

void HudRenderer::render() const {

    // Consola (arriba izquierda)
    renderer.Copy(texture_manager.get_texture("hud_console_base"), SDL2pp::NullOpt,
                  SDL2pp::Rect(CONSOLE_X, CONSOLE_Y, CONSOLE_W, CONSOLE_H));
    // Upper layer (full screen overlay)
    renderer.Copy(texture_manager.get_texture("hud_upper_layer"), SDL2pp::NullOpt,
                  SDL2pp::Rect(0, 0, w_width, w_height));

    // User info (arriba derecha)
    renderer.Copy(texture_manager.get_texture("hud_user_info_base"), SDL2pp::NullOpt,
                  SDL2pp::Rect(PANEL_RIGHT_X, USER_INFO_Y, PANEL_RIGHT_W, USER_INFO_H));

    // Inventario
    renderer.Copy(texture_manager.get_texture("hud_inventory_base"), SDL2pp::NullOpt,
                  SDL2pp::Rect(PANEL_RIGHT_X, INVENTORY_Y, INVENTORY_W, INVENTORY_H));
    render_inventory();

    // Stats (vida/mana)
    renderer.Copy(texture_manager.get_texture("hud_stats_base"), SDL2pp::NullOpt,
                  SDL2pp::Rect(PANEL_RIGHT_X, STATS_Y, STATS_W, STATS_H));

    render_progress_bar(PROGRESS_BAR_X, HP_BAR_Y, PROGRESS_BAR_W, PROGRESS_BAR_H, stats.hp,
                        stats.max_hp, SDL_Color{190, 45, 45, 255});
    render_progress_bar(PROGRESS_BAR_X, MANA_BAR_Y, PROGRESS_BAR_W, PROGRESS_BAR_H, stats.mana,
                        stats.max_mana, SDL_Color{45, 95, 205, 255});
    render_progress_bar(PROGRESS_BAR_X, EXP_BAR_Y, PROGRESS_BAR_W, PROGRESS_BAR_H, stats.exp,
                        stats.exp_next_level, SDL_Color{210, 170, 45, 255});

    render_centered_text(player_name_texture, NAME_X, NAME_Y, NAME_W, NAME_H);
    render_centered_text(level_texture, LEVEL_X, LEVEL_Y, LEVEL_W, LEVEL_H);
    render_centered_text(hp_texture, PROGRESS_BAR_X, HP_BAR_Y, PROGRESS_BAR_W, PROGRESS_BAR_H);
    render_centered_text(mana_texture, PROGRESS_BAR_X, MANA_BAR_Y, PROGRESS_BAR_W, PROGRESS_BAR_H);
    render_centered_text(exp_texture, PROGRESS_BAR_X, EXP_BAR_Y, PROGRESS_BAR_W, PROGRESS_BAR_H);

    render_chat();

    SDL_SetRenderDrawColor(renderer.Get(), 0, 0, 0, 255);
}
