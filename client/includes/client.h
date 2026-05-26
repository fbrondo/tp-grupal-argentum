#pragma once
#include <cstdint>
#include <optional>

#include <SDL2/SDL.h>
#include <SDL2pp/SDL2pp.hh>
#include <SDL2pp/SDLImage.hh>

#include "common/includes/queue.h"
#include "common/includes/socket.h"

#include "client_protocol.h"

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

enum class Direction { DOWN = 0, UP = 1, LEFT = 2, RIGHT = 3 };

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
    float player_x = 390.0f;
    float player_y = 280.0f;
    Direction player_dir = Direction::DOWN;

    // Socket skt;
    // ClientProtocol protocol;

    // Queue<ClientMessageDTO> events_queue;
    // Queue<ServerMessageDTO> server_queue;
    // Sender sender;
    // Receiver receiver;

    void init_SDL();
    void update_state_from_server();
    void handle_events();
    void clear_display();
    void render_in_z_order();
    uint32_t sleep_and_calc_next_it(uint32_t frame_start) const;

public:
    Client();  //(const char* host, const char* port);
    void launch();
};
