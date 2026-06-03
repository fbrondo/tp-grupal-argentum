#pragma once

#include <memory>

#include <SDL2pp/SDL.hh>
#include <SDL2pp/SDLImage.hh>

#include "client/includes/texture_manager.h"
#include "client/includes/window/windowSDL.h"
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

static constexpr int TARGET_FPS = 60;
static constexpr int FRAME_MS = 1000 / TARGET_FPS;
static constexpr int TILE_SIZE = 32;

struct PlayerPosition {
    uint32_t pos_x;
    uint32_t pos_y;
    uint8_t dir;
};

class Client {
private:
    Socket skt;
    ClientProtocol protocol;

    Queue<EventClient> events_queue;
    Queue<std::unique_ptr<ClientCommand>> cmd_queue;
    ClientSender sender;
    ClientReceiver receiver;

    SDLImage img;
    WindowSDL window;
    TextureManager texture_manager;
    PlayerPosition player_state;

    bool is_running = true;
    uint32_t it = 0;

    // void init_SDL();
    void update_state_from_server();
    void handle_events();
    void clear_display();
    void render_in_z_order();
    uint32_t sleep_and_calc_next_it(uint32_t frame_start) const;
    void close();

public:
    Client(const char* host, const char* port);
    void launch();
};
