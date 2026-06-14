#include "client/includes/client.h"

#include <SDL2/SDL_image.h>

#include "client/includes/commands/command_move.h"

Client::Client(const char* host, const char* port):
        skt(host, port),
        protocol(this->skt),
        sender(protocol, cmd_queue),
        receiver(protocol, events_queue),
        img(IMG_INIT_JPG | IMG_INIT_PNG),
        window("Argentum Online"),
        texture_manager(window.get_renderer(), window),
        world_renderer(window.get_renderer(), texture_manager) {}

void Client::handle_events() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            is_running = false;
            return;
        }
        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
            is_running = false;
            return;
        }
    }
}

void Client::process_movement_input() {
    const uint8_t* keys = SDL_GetKeyboardState(nullptr);
    Direction direction = DOWN;
    bool movement_key_down = true;

    if (keys[SDL_SCANCODE_DOWN] || keys[SDL_SCANCODE_S]) {
        direction = DOWN;
    } else if (keys[SDL_SCANCODE_UP] || keys[SDL_SCANCODE_W]) {
        direction = UP;
    } else if (keys[SDL_SCANCODE_LEFT] || keys[SDL_SCANCODE_A]) {
        direction = LEFT;
    } else if (keys[SDL_SCANCODE_RIGHT] || keys[SDL_SCANCODE_D]) {
        direction = RIGHT;
    } else {
        movement_key_down = false;
    }

    if (!movement_key_down) {
        movement_key_was_down = false;
        return;
    }

    const uint32_t now = SDL_GetTicks();
    const bool direction_changed = movement_key_was_down && direction != last_move_direction;
    const bool repeat_due = now - last_move_command_ticks >= MOVE_REPEAT_MS;

    if (!movement_key_was_down || direction_changed || repeat_due) {
        cmd_queue.push(std::make_unique<MoveCommandClient>(direction));
        last_move_command_ticks = now;
        last_move_direction = direction;
    }
    movement_key_was_down = true;
}

void Client::clear_display() { window.clear(); }

float Client::calculate_delta_time() {
    const uint32_t current_ticks = SDL_GetTicks();
    const float dt = static_cast<float>(current_ticks - last_frame_ticks) / 1000.0f;
    last_frame_ticks = current_ticks;
    return dt;
}

void Client::render_in_z_order() {
    world_renderer.render();
    window.present();
}

void Client::update_state_from_server() {
    EventClient event;
    while (events_queue.try_pop(event)) {
        switch (event.type) {
            case TypeEventClient::UPDATE_WORLD: {
                world_renderer.update_from_snapshot(event.world);
                break;
            }
            case TypeEventClient::MAP_DATA: {
                world_renderer.load_map(std::move(event.map_data));
                break;
            }
            case TypeEventClient::DISCONNECTION:
                is_running = false;
                break;
            default:
                break;
        }
    }
}

uint32_t Client::sleep_and_calc_next_it(const uint32_t frame_start) const {
    const uint32_t current_ticks = SDL_GetTicks();
    const uint32_t elapsed = current_ticks - frame_start;

    if (elapsed < static_cast<uint32_t>(FRAME_MS)) {
        SDL_Delay(FRAME_MS - elapsed);
        return it + 1;
    }
    const uint32_t frames_passed = (elapsed / FRAME_MS);
    return it + std::max(1u, frames_passed);
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

void Client::launch(const std::string& user, const std::string& pass) {
    try {
        if (!user.empty()) {
            protocol.sendLogin(user, pass);
            EventClient login_event;
            while (protocol.receiveMessage(login_event)) {
                if (login_event.type == TypeEventClient::LOGIN_RESPONSE) {
                    if (login_event.login_success) {
                        world_renderer.set_local_player(login_event.player_id);
                    }
                    break;
                }
                if (login_event.type == TypeEventClient::MAP_DATA) {
                    world_renderer.load_map(std::move(login_event.map_data));
                }
            }
        }
        sender.start();
        receiver.start();
        last_frame_ticks = SDL_GetTicks();
        while (is_running) {
            if (!receiver.is_alive() || !sender.is_alive()) {
                break;
            }
            const float dt = calculate_delta_time();
            const uint32_t frame_start = last_frame_ticks;

            update_state_from_server();
            handle_events();
            process_movement_input();
            world_renderer.update_animations(dt);
            clear_display();
            render_in_z_order();
            it = sleep_and_calc_next_it(frame_start);
        }
        close();
    } catch (const std::exception& e) {
        close();
    }
}
