#pragma once

#include <memory>
#include <string>

#include <SDL2pp/SDL.hh>
#include <SDL2pp/SDLImage.hh>
#include <SDL2pp/SDLTTF.hh>

#include "client/includes/chat_manager.h"
#include "client/includes/core/window_config.h"
#include "client/includes/texture_manager.h"
#include "client/includes/window/windowSDL.h"
#include "client/includes/world_renderer.h"
#include "commands/command_client.h"
#include "common/includes/queue.h"
#include "common/includes/socket.h"

#include "client_protocol.h"
#include "client_receiver.h"
#include "client_sender.h"

using SDL2pp::Rect;
using SDL2pp::Renderer;
using SDL2pp::SDL;
using SDL2pp::SDLImage;
using SDL2pp::Surface;
using SDL2pp::Texture;

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
    WorldRenderer world_renderer;

    uint32_t last_frame_ticks = 0;
    uint32_t last_move_command_ticks = 0;
    uint32_t it = 0;
    bool is_running = true;
    Direction last_move_direction = DOWN;

    ChatManager chat;

    static WindowConfig loadWindowConfig();
    void update_state_from_server();
    void handle_events();
    void process_movement_input();
    void clear_display();
    float calculate_delta_time();
    void render_in_z_order();
    uint32_t sleep_and_calc_next_it(uint32_t frame_start) const;
    void close();

public:
    Client(const char* host, const char* port);
    void launch(const std::string& user = "", const std::string& pass = "");
};
