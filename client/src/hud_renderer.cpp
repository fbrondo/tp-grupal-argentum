#include "../includes/hud_renderer.h"

#include <algorithm>
#include <string>

#include "client/includes/chat_manager.h"

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
        default:
            sdl_color = {255, 255, 255, 255};
            break;
    }
    chat_log_textures.push_back(create_text_texture_colored(msg, sdl_color));

    if (chat_log_textures.size() > 6) {
        chat_log_textures.pop_front();
    }
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
    int start_y = CONSOLE_Y + 5;
    int line_spacing = 18;

    for (const auto& tex: chat_log_textures) {
        if (tex) {
            renderer.Copy(*tex, SDL2pp::NullOpt,
                          SDL2pp::Rect(CONSOLE_X + 10, start_y, tex->GetWidth(), tex->GetHeight()));
            start_y += line_spacing;
        }
    }

    if (chat_is_active && chat_input_texture) {
        int input_y = CONSOLE_Y + CONSOLE_H - 22;
        renderer.Copy(*chat_input_texture, SDL2pp::NullOpt,
                      SDL2pp::Rect(CONSOLE_X + 10, input_y, chat_input_texture->GetWidth(),
                                   chat_input_texture->GetHeight()));
    }
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
