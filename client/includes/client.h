#pragma once

#include <memory>
#include <string>

#include <SDL2pp/SDL.hh>
#include <SDL2pp/SDLImage.hh>
#include <SDL2pp/SDLTTF.hh>

#include "client/includes/chat_manager.h"
#include "client/includes/core/window_config.h"
#include "client/includes/font_manager.h"
#include "client/includes/texture_manager.h"
#include "client/includes/window/windowSDL.h"
#include "client/includes/world_renderer.h"
#include "commands/command_client.h"
#include "common/includes/queue.h"
#include "common/includes/socket.h"

#include "client_protocol.h"
#include "client_receiver.h"
#include "client_sender.h"
#include "sound_manager.h"

using SDL2pp::SDL;
using SDL2pp::SDLImage;

class Client {
private:
    Socket skt;
    ClientProtocol protocol;

    Queue<EventClient> events_queue;
    Queue<std::unique_ptr<ClientCommand>> cmd_queue;
    ClientSender sender;
    ClientReceiver receiver;

    SDLImage img;
    SDL2pp::SDLTTF ttf;
    WindowSDL window;
    TextureManager texture_manager;
    FontManager font_manager;
    WorldRenderer world_renderer;

    uint32_t last_frame_ticks = 0;
    uint32_t last_move_command_ticks = 0;
    uint32_t frame_count = 0;
    bool is_running = true;
    Direction last_move_direction = DOWN;

    ChatManager chat;
    std::optional<uint8_t> selected_inv_slot;

    // --- Config ---
    static WindowConfig loadWindowConfig();

    // --- Login ---
    void handle_login_sequence(const std::string& user, const std::string& pass);
    void handle_login_success(uint32_t player_id);

    // --- Server events ---
    void update_state_from_server();
    void handle_chat_event(const EventClient& event);
    void handle_merchant_event(const EventClient& event);
    void handle_bank_event(const EventClient& event);

    // --- Input ---
    void handle_events();
    void handle_keyboard_event(const SDL_Event& event);
    void handle_mouse_event(const SDL_Event& event);
    void handle_chat_input_event(const SDL_Event& event);
    void handle_left_click(uint32_t mouse_x, uint32_t mouse_y);
    void process_movement_input();

    // --- UI ---
    void sync_chat_ui();
    void process_help_command();

    // --- Game loop ---
    void clear_display();
    float calculate_delta_time();
    void render_in_z_order();
    uint32_t sleep_and_calc_next_frame(uint32_t frame_start) const;

    // --- Cleanup ---
    void close();

public:
    Client(const char* host, const char* port);
    void launch(const std::string& user = "", const std::string& pass = "");
};
