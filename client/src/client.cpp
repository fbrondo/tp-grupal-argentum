#include "client/includes/client.h"

#include <SDL2/SDL_image.h>

Client::Client() =
        default;  //(const char* host, const char* port): skt(host, port), protocol(this->skt) {};

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
            switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                    is_running = false;
                    break;
                case SDLK_DOWN:
                case SDLK_s:
                    player_dir = Direction::DOWN;
                    player_y += BODY_H;
                    break;
                case SDLK_UP:
                case SDLK_w:
                    player_dir = Direction::UP;
                    player_y -= BODY_H;
                    break;
                case SDLK_LEFT:
                case SDLK_a:
                    player_dir = Direction::LEFT;
                    player_x -= BODY_W;
                    break;
                case SDLK_RIGHT:
                case SDLK_d:
                    player_dir = Direction::RIGHT;
                    player_x += BODY_W;
                    break;
                default:
                    break;
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
    Rect dst_body(static_cast<int>(player_x), static_cast<int>(player_y), BODY_W, BODY_H);
    renderer->Copy(*body_tex, src_body, dst_body);

    Rect src_head(0, 0, HEAD_W, HEAD_H);
    Rect dst_head(static_cast<int>(player_x) + (BODY_W - HEAD_W) / 2,
                  static_cast<int>(player_y) - HEAD_H + 3, HEAD_W, HEAD_H);
    renderer->Copy(*head_tex, src_head, dst_head);

    renderer->Present();
}

void Client::update_state_from_server() {
    // TODO: actualizar player_x, player_y desde WorldState cuando haya servidor
}

uint32_t Client::sleep_and_calc_next_it(const uint32_t frame_start) const {
    if (const uint32_t elapsed = SDL_GetTicks() - frame_start;
        elapsed < static_cast<uint32_t>(FRAME_MS)) {
        SDL_Delay(FRAME_MS - elapsed);
        return it + 1;
    }
    return it + (SDL_GetTicks() - frame_start) / FRAME_MS;
}

void Client::launch() {
    init_SDL();
    while (is_running) {
        const uint32_t frame_start = SDL_GetTicks();
        update_state_from_server();
        handle_events();
        clear_display();
        render_in_z_order();
        it = sleep_and_calc_next_it(frame_start);
    }
}
