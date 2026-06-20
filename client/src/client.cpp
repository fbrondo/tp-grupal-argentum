#include "client/includes/client.h"

#include <algorithm>
#include <cctype>
#include <string>
#include <unordered_map>

#include <SDL2/SDL_image.h>

#include "client/includes/commands/command_attack.h"
#include "client/includes/commands/command_chat.h"
#include "client/includes/commands/command_move.h"
#include "client/includes/core/constants.h"
#include "common/includes/toml_config.h"

static const char* item_name(uint8_t type) {
    switch (type) {
        case SWORD:
            return "Espada";
        case AXE:
            return "Hacha";
        case HAMMER:
            return "Martillo";
        case ASH_STAFF:
            return "Vara de fresno";
        case ELVEN_FLUTE:
            return "Flauta elfica";
        case KNOTTED_STAFF:
            return "Baculo nudoso";
        case INLAID_STAFF:
            return "Baculo engarzado";
        case SIMPLE_BOW:
            return "Arco simple";
        case COMPOUND_BOW:
            return "Arco compuesto";
        case LEATHER_ARMOR:
            return "Armadura de cuero";
        case PLATE_AMOR:
            return "Armadura de placas";
        case BLUE_TUNIC:
            return "Tunica azul";
        case HOOD:
            return "Capucha";
        case IRON_HELMET:
            return "Casco de hierro";
        case TORTOISE_SHIELD:
            return "Escudo de tortuga";
        case IRON_SHIELD:
            return "Escudo de hierro";
        case MAGIC_HAT:
            return "Sombrero magico";
        case LIFE_POTION:
            return "Pocion vida";
        case MANA_POTION:
            return "Pocion mana";
        case GOLD:
            return "Oro";
        default:
            return "?";
    }
}

static bool get_pressed_movement_direction(Direction& direction) {
    SDL_PumpEvents();
    if (SDL_GetKeyboardFocus() == nullptr) {
        return false;
    }

    const Uint8* keyboard_state = SDL_GetKeyboardState(nullptr);
    if (keyboard_state[SDL_SCANCODE_UP] || keyboard_state[SDL_SCANCODE_W]) {
        direction = UP;
        return true;
    }
    if (keyboard_state[SDL_SCANCODE_LEFT] || keyboard_state[SDL_SCANCODE_A]) {
        direction = LEFT;
        return true;
    }
    if (keyboard_state[SDL_SCANCODE_RIGHT] || keyboard_state[SDL_SCANCODE_D]) {
        direction = RIGHT;
        return true;
    }
    if (keyboard_state[SDL_SCANCODE_DOWN] || keyboard_state[SDL_SCANCODE_S]) {
        direction = DOWN;
        return true;
    }

    return false;
}

WindowConfig Client::loadWindowConfig() {
    WindowConfig cfg;
    try {
        TomlConfig toml_cfg("config.toml");
        cfg.fullscreen = toml_cfg.get_or<bool>("window.fullscreen", false);
        cfg.width = toml_cfg.get_or<int>("window.width", WINDOW_W);
        cfg.height = toml_cfg.get_or<int>("window.height", WINDOW_H);
    } catch (const std::exception& e) {
        std::cerr << "No se pudo cargar config.toml, usando valores por defecto: " << e.what()
                  << std::endl;
    }
    return cfg;
}

Client::Client(const char* host, const char* port):
        skt(host, port),
        protocol(this->skt),
        sender(protocol, cmd_queue),
        receiver(protocol, events_queue),
        img(IMG_INIT_JPG | IMG_INIT_PNG),
        ttf(),
        window([] {
            const WindowConfig cfg = loadWindowConfig();
            return WindowSDL("Argentum Online", cfg.width, cfg.height, cfg.fullscreen);
        }()),
        texture_manager(window.get_renderer(), window),
        world_renderer(window.get_renderer(), texture_manager, font_manager) {
    SoundManager::init();
}

void Client::sync_chat_ui() {
    world_renderer.update_chat_input(chat.get_buffer(), chat.is_active());
}

void Client::handle_left_click(uint32_t mouse_x, uint32_t mouse_y) {
    constexpr SDL_Rect world_view = {7, 149, 672, 384};
    const bool in_world = mouse_x >= world_view.x && mouse_x < world_view.x + world_view.w &&
                          mouse_y >= world_view.y && mouse_y < world_view.y + world_view.h;

    if (chat.is_active()) {
        if (!world_renderer.is_point_inside_console(mouse_x, mouse_y)) {
            chat.set_active(false);
            sync_chat_ui();
        }
    } else {
        if (in_world) {
            auto hit = world_renderer.get_entity_at_screen(mouse_x, mouse_y);
            if (hit) {
                auto [entity_id, entity_type] = *hit;
                if (entity_type == EntityType::PLAYER) {
                    cmd_queue.push(std::make_unique<AttackCommandClient>(entity_id));
                } else if (entity_type == EntityType::NPC) {
                    cmd_queue.push(std::make_unique<AttackCommandClient>(entity_id));
                } else if (entity_type == EntityType::CITIZEN) {
                    chat.select_npc(entity_id);
                    world_renderer.set_citizen_selected(entity_id);
                }
            } else {
                chat.clear_npc_selection();
                world_renderer.set_citizen_selected(-1);
            }
        }

        if (world_renderer.is_point_inside_console(mouse_x, mouse_y)) {
            chat.set_active(true);
            sync_chat_ui();
        }
    }
}

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
        if (!chat.is_active() && event.type == SDL_KEYDOWN && event.key.repeat == 0 &&
            event.key.keysym.sym == SDLK_F5) {
            cmd_queue.push(std::make_unique<ChatCommandClient>("/debug_morir"));
        }
        if (!chat.is_active() && event.type == SDL_KEYDOWN && event.key.repeat == 0 &&
            event.key.keysym.sym == SDLK_F6) {
            cmd_queue.push(std::make_unique<ChatCommandClient>("/resucitar"));
        }
        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_F11) {
            window.toggle_fullscreen();
        }
        if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_FOCUS_LOST) {
            last_move_command_ticks = 0;
        }

        if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
            handle_left_click(event.button.x, event.button.y);
        }

        if (event.type == SDL_MOUSEWHEEL) {
            int mouse_x, mouse_y;
            SDL_GetMouseState(&mouse_x, &mouse_y);
            if (world_renderer.is_point_inside_console(mouse_x, mouse_y)) {
                world_renderer.scroll_console(-event.wheel.y);
            }
        }

        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RETURN) {
            if (chat.is_active()) {
                std::string msg = chat.extract_message();
                if (!msg.empty()) {
                    uint32_t npc_id = chat.has_npc_selection() ? chat.get_selected_npc_id() : 0;
                    cmd_queue.push(std::make_unique<ChatCommandClient>(msg, npc_id));
                    world_renderer.set_chat_bubble_on_local(msg);
                }
                chat.set_active(false);
                chat.clear_npc_selection();
                world_renderer.set_citizen_selected(-1);
                sync_chat_ui();
            }
        }

        if (chat.is_active() && event.type == SDL_TEXTINPUT) {
            chat.append_text(event.text.text);
            sync_chat_ui();
        }

        if (chat.is_active() && event.type == SDL_KEYDOWN &&
            event.key.keysym.sym == SDLK_BACKSPACE) {
            chat.remove_last_char();
            sync_chat_ui();
        }
    }
}

void Client::process_movement_input() {
    if (chat.is_active()) {
        last_move_command_ticks = 0;
        return;
    }

    Direction direction = last_move_direction;
    if (!get_pressed_movement_direction(direction)) {
        last_move_command_ticks = 0;
        return;
    }

    const uint32_t now = SDL_GetTicks();
    const bool movement_started = last_move_command_ticks == 0;
    const bool direction_changed = direction != last_move_direction;
    const bool repeat_due = now - last_move_command_ticks >= MOVE_REPEAT_MS;

    if (world_renderer.is_local_player_moving()) {
        return;
    }

    if (movement_started || direction_changed || repeat_due) {
        cmd_queue.push(std::make_unique<MoveCommandClient>(direction));
        last_move_command_ticks = now;
        last_move_direction = direction;
    }
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
                world_renderer.load_map(std::move(event.map_data), event.citizens);
                break;
            }
            case TypeEventClient::OWN_STATS:
                world_renderer.update_hud_stats(event.stats);
                break;
            case TypeEventClient::CHAT_MSG: {
                auto parsed = ChatManager::parse_server_message(event.text_payload);
                switch (parsed.type) {
                    case ParsedChatMessage::PUBLIC:
                    case ParsedChatMessage::WHISPER_RECEIVED:
                        world_renderer.set_chat_bubble_on_player(parsed.sender_name, parsed.text);
                        break;
                    case ParsedChatMessage::WHISPER_SENT:
                        world_renderer.set_chat_bubble_on_local(parsed.text);
                        break;
                    case ParsedChatMessage::SYSTEM:
                        chat.add_message_to_log(parsed.text);
                        world_renderer.add_chat_message(parsed.text, parsed.color);
                        break;
                }
                break;
            }
            case TypeEventClient::OPEN_MERCHANT: {
                world_renderer.add_chat_message("--- Catalogo del comerciante ---", COLOR_BLUE);
                for (const auto& [type, price]: event.merchant_data.catalog) {
                    world_renderer.add_chat_message(
                            std::string(item_name(static_cast<uint8_t>(type))) + " - " +
                                    std::to_string(price) + "g",
                            COLOR_WHITE);
                }
                break;
            }
            case TypeEventClient::OPEN_BANK: {
                world_renderer.add_chat_message("--- Contenido del banco ---", COLOR_BLUE);
                world_renderer.add_chat_message("Oro: " + std::to_string(event.bank_data.gold),
                                                COLOR_YELLOW);
                for (const auto& item: event.bank_data.items) {
                    world_renderer.add_chat_message(std::string(item_name(item.item_type)),
                                                    COLOR_WHITE);
                }
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
        last_move_command_ticks = 0;
        if (!user.empty()) {
            world_renderer.set_player_name(user);
            protocol.sendLogin(user, pass);
            EventClient login_event;
            while (protocol.receiveMessage(login_event)) {
                if (login_event.type == TypeEventClient::LOGIN_RESPONSE) {
                    if (login_event.login_success) {
                        world_renderer.set_local_player(login_event.player_id);
                        world_renderer.add_chat_message("¡Bienvenido a las Tierras de Argentum!",
                                                        COLOR_BLUE);
                    }
                    break;
                }
                if (login_event.type == TypeEventClient::MAP_DATA) {
                    world_renderer.load_map(std::move(login_event.map_data), login_event.citizens);
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
