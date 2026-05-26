#include "client/includes/client.h"

#include <SDL2/SDL_image.h>

#include "client/includes/commands/command_move.h"

Client::Client(const char* host, const char* port):
        skt(host, port),
        protocol(this->skt),
        sender(protocol, cmd_queue),
        receiver(protocol, events_queue) {}

void Client::init_SDL() {
    // El orden importa: SDL primero, luego imagen, luego ventana, luego renderer, luego texturas
    sdl.emplace(SDL_INIT_VIDEO);
    img.emplace(IMG_INIT_PNG);
    window.emplace("Argentum Online", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_W,
                   WINDOW_H, SDL_WINDOW_RESIZABLE);

    if (const Surface icon(IMG_Load("client/assets/icon.png")); icon.Get()) {
        window->SetIcon(icon);
    }

    renderer.emplace(*window, -1, SDL_RENDERER_ACCELERATED);

    // Color key negro para transparencia (fondo negro del PNG → transparente)
    Surface body(IMG_Load("client/assets/body.png"));
    if (!body.Get()) {
        throw std::runtime_error(std::string("No se pudo cargar body.png: ") + IMG_GetError());
    }
    SDL_SetColorKey(body.Get(), SDL_TRUE, SDL_MapRGB(body.Get()->format, 0, 0, 0));
    body_tex.emplace(*renderer, Surface(std::move(body)));

    Surface head(IMG_Load("client/assets/head.png"));
    if (!head.Get()) {
        throw std::runtime_error(std::string("No se pudo cargar head.png: ") + IMG_GetError());
    }
    SDL_SetColorKey(head.Get(), SDL_TRUE, SDL_MapRGB(head.Get()->format, 0, 0, 0));
    head_tex.emplace(*renderer, Surface(std::move(head)));
}

void Client::handle_events() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            is_running = false;
            return;
        }
        if (event.type == SDL_KEYDOWN) {
            Direction dir;
            bool is_move = true;

            switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                    is_running = false;
                    return;
                case SDLK_DOWN:
                case SDLK_s:
                    dir = DOWN;
                    break;
                case SDLK_UP:
                case SDLK_w:
                    dir = UP;
                    break;
                case SDLK_LEFT:
                case SDLK_a:
                    dir = LEFT;
                    break;
                case SDLK_RIGHT:
                case SDLK_d:
                    dir = RIGHT;
                    break;
                default:
                    is_move = false;
                    break;
            }

            if (is_move) {
                // El comando viaja por la queue al ClientSender,
                // que llama a protocol.sendMove(). El estado local
                // solo se actualiza cuando llega el snapshot del servidor.
                auto cmd = std::make_unique<MoveCommandClient>(dir);
                cmd_queue.push(std::move(cmd));
            }
        }
    }
}

void Client::clear_display() {
    renderer->SetDrawColor(34, 139, 34, 255);
    renderer->Clear();
}

void Client::render_in_z_order() {
    Rect src_body(0, 0, BODY_W, BODY_H);
    Rect dst_body(static_cast<int>(player_state.pos_x), static_cast<int>(player_state.pos_y),
                  BODY_W, BODY_H);
    renderer->Copy(*body_tex, src_body, dst_body);

    Rect src_head(0, 0, HEAD_W, HEAD_H);
    Rect dst_head(static_cast<int>(player_state.pos_x) + (BODY_W - HEAD_W) / 2,
                  static_cast<int>(player_state.pos_y) - HEAD_H + 3, HEAD_W, HEAD_H);
    renderer->Copy(*head_tex, src_head, dst_head);

    renderer->Present();
}

void Client::update_state_from_server() {
    EventClient event;
    while (events_queue.try_pop(event)) {
        if (event.type == TypeEventClient::DISCONNECTION) {
            is_running = false;
            return;
        }
        if (event.type == TypeEventClient::UPDATE_WORLD && !event.world.players.empty()) {
            player_state.pos_x = event.world.players[0].pos_x;
            player_state.pos_y = event.world.players[0].pos_y;
            player_state.dir = event.world.players[0].direction;
        }
    }
}

uint32_t Client::sleep_and_calc_next_it(const uint32_t frame_start) const {
    if (const uint32_t elapsed = SDL_GetTicks() - frame_start;
        elapsed < static_cast<uint32_t>(FRAME_MS)) {
        SDL_Delay(FRAME_MS - elapsed);
        return it + 1;
    }
    return it + (SDL_GetTicks() - frame_start) / FRAME_MS;
}

void Client::close() {
    skt.shutdown(2);
    skt.close();
    events_queue.close();
    cmd_queue.close();
    receiver.stop();
    sender.stop();
    receiver.join();
    sender.join();
}

void Client::launch() {
    try {
        init_SDL();
        sender.start();
        receiver.start();
        while (is_running) {
            if (!receiver.is_alive() || !sender.is_alive()) {
                break;  // En caso de que alguno de los hilos falle, salir del loop
            }
            const uint32_t frame_start = SDL_GetTicks();
            update_state_from_server();
            handle_events();
            clear_display();
            render_in_z_order();
            it = sleep_and_calc_next_it(frame_start);
        }
        close();
    } catch (const std::exception& e) {
        close();
    }
}
