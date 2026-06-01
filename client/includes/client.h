#pragma once
#include <cstdint>
#include <memory>
#include <optional>

#include <SDL2/SDL.h>
#include <SDL2pp/SDL2pp.hh>
#include <SDL2pp/SDLImage.hh>

#include "commands/command_client.h"
#include "common/includes/queue.h"
#include "common/includes/socket.h"
#include "common/includes/direction.h"

#include "client_protocol.h"
#include "client_receiver.h"
#include "client_sender.h"

using SDL2pp::Rect;
using SDL2pp::Renderer;
using SDL2pp::SDL;
using SDL2pp::SDLImage;
using SDL2pp::Surface;
using SDL2pp::Texture;
using SDL2pp::Window;

static constexpr int WINDOW_W = 800;
static constexpr int WINDOW_H = 600;
static constexpr int TARGET_FPS = 60;
static constexpr int FRAME_MS = 1000 / TARGET_FPS;
static constexpr int BODY_W = 19;
static constexpr int BODY_H = 37;
static constexpr int HEAD_W = 13;
static constexpr int HEAD_H = 14;

//enum Direction : uint8_t { DOWN, UP, LEFT, RIGHT };

struct PlayerPosition {
    uint32_t pos_x;
    uint32_t pos_y;
    uint8_t dir;
};

class Client {
private:
    std::optional<SDL> sdl;
    std::optional<SDLImage> img;
    std::optional<Window> window;
    std::optional<Renderer> renderer;
    std::optional<Texture> body_tex;
    std::optional<Texture> head_tex;

    bool is_running = true;
    uint32_t it = 0;

    Socket skt;
    ClientProtocol protocol;

    Queue<EventClient> events_queue;
    Queue<std::unique_ptr<ClientCommand>> cmd_queue;
    ClientSender sender;
    ClientReceiver receiver;

    PlayerPosition player_state;

    void init_SDL();
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
