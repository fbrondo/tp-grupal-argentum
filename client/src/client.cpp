#include "client/includes/client.h"

#include <algorithm>
#include <cctype>
#include <string>

#include <SDL2/SDL_image.h>

#include "client/includes/commands/command_attack.h"
#include "client/includes/commands/command_chat.h"
#include "client/includes/commands/command_equip.h"
#include "client/includes/commands/command_move.h"
#include "client/includes/commands/command_unequip.h"
#include "client/includes/core/constants.h"
#include "client/includes/core/item_names.h"
#include "client/includes/input_utils.h"
#include "common/includes/toml_config.h"

// --- Config --- //

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

// --- Constructor --- //

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
    if (SoundManager::init()) {
        SoundManager::play_background_music(ARGENTUM_SHARE_PATH "/client/assets/Sounds/31.mp3",
                                            3.0);
    }
}

// --- Login --- //

void Client::handle_login_success(uint32_t player_id) {
    world_renderer.set_local_player(player_id);
    world_renderer.add_chat_message("¡Bienvenido a las Tierras de Argentum!", COLOR_BLUE);
    world_renderer.add_chat_message("Escribi /help para ver los comandos disponibles.",
                                    COLOR_YELLOW);
}

void Client::handle_login_sequence(const std::string& user, const std::string& pass) {
    world_renderer.set_player_name(user);
    protocol.sendLogin(user, pass);
    EventClient login_event;
    while (protocol.receiveMessage(login_event)) {
        if (login_event.type == TypeEventClient::LOGIN_RESPONSE) {
            if (login_event.login_success) {
                handle_login_success(login_event.player_id);
            }
            break;
        }
        if (login_event.type == TypeEventClient::MAP_DATA) {
            world_renderer.load_map(std::move(login_event.map_data), login_event.citizens);
        }
        if (login_event.type == TypeEventClient::INVENTORY_UPDATE) {
            world_renderer.update_hud_inventory(login_event.inventory);
        }
    }
}

// --- Server events --- //

void Client::handle_chat_event(const EventClient& event) {
    auto parsed = ChatManager::parse_server_message(event.text_payload);
    switch (parsed.type) {
        case ParsedChatMessage::PUBLIC:
            world_renderer.set_chat_bubble_on_player(parsed.sender_name, parsed.text);
            break;
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
}

void Client::handle_merchant_event(const EventClient& event) {
    world_renderer.add_chat_message("--- Catalogo del comerciante ---", COLOR_YELLOW);
    for (const auto& [type, entry]: event.merchant_data.catalog) {
        std::string name = item_name(type);
        std::string line = name + ": " + std::to_string(entry.purchase_price) + " compra - " +
                           std::to_string(entry.selling_price) + " venta";
        world_renderer.add_chat_message(line, COLOR_WHITE);
    }
}

void Client::handle_bank_event(const EventClient& event) {
    world_renderer.add_chat_message("--- Contenido del banco ---", COLOR_YELLOW);
    world_renderer.add_chat_message("Oro: " + std::to_string(event.bank_data.gold), COLOR_YELLOW);
    if (!event.bank_data.items.empty()) {
        for (const auto& [type, quantity]: event.bank_data.items) {
            std::string name = item_name(type);
            std::string line = name + ": " + std::to_string(quantity);
            world_renderer.add_chat_message(line, COLOR_WHITE);
        }
    }
}

void Client::update_state_from_server() {
    EventClient event;
    while (events_queue.try_pop(event)) {
        switch (event.type) {
            case TypeEventClient::UPDATE_WORLD:
                world_renderer.update_from_snapshot(event.world);
                break;
            case TypeEventClient::MAP_DATA:
                world_renderer.load_map(std::move(event.map_data), event.citizens);
                break;
            case TypeEventClient::OWN_STATS:
                world_renderer.update_hud_stats(event.stats);
                break;
            case TypeEventClient::INVENTORY_UPDATE:
                world_renderer.update_hud_inventory(event.inventory);
                break;
            case TypeEventClient::EQUIPMENT_UPDATE:
                world_renderer.update_hud_equipment(event.equipment);
                break;
            case TypeEventClient::CHAT_MSG:
                handle_chat_event(event);
                break;
            case TypeEventClient::OPEN_MERCHANT:
                handle_merchant_event(event);
                break;
            case TypeEventClient::OPEN_BANK:
                handle_bank_event(event);
                break;
            case TypeEventClient::DISCONNECTION:
                is_running = false;
                break;
            default:
                break;
        }
    }
}

// --- Input --- //

void Client::handle_keyboard_event(const SDL_Event& event) {
    if (event.type == SDL_QUIT ||
        (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) {
        is_running = false;
        return;
    }
    if (!chat.is_active() && event.type == SDL_KEYDOWN && event.key.repeat == 0) {
        if (event.key.keysym.sym == SDLK_F5) {
            cmd_queue.push(std::make_unique<ChatCommandClient>("/debug_morir"));
        } else if (event.key.keysym.sym == SDLK_F7) {
            cmd_queue.push(std::make_unique<ChatCommandClient>("/debug_vida_infinita"));
        } else if (event.key.keysym.sym == SDLK_F8) {
            cmd_queue.push(std::make_unique<ChatCommandClient>("/debug_mana_infinito"));
        }
    }
    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_F11) {
        window.toggle_fullscreen();
    }
    if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_FOCUS_LOST) {
        last_move_command_ticks = 0;
    }
    if (event.type == SDL_KEYDOWN && event.key.repeat == 0 && event.key.keysym.sym == SDLK_c &&
        (event.key.keysym.mod & KMOD_CTRL)) {
        chat.set_active(!chat.is_active());
        sync_chat_ui();
    }
}

void Client::handle_mouse_event(const SDL_Event& event) {
    if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
        const uint32_t mouse_x = event.button.x;
        const uint32_t mouse_y = event.button.y;

        if (event.button.clicks >= 2) {
            const auto equipped_slot_index = world_renderer.equipment_slot_at(mouse_x, mouse_y);
            if (equipped_slot_index.has_value()) {
                cmd_queue.push(std::make_unique<UnequipCommandClient>(equipped_slot_index.value()));
                return;
            }

            const auto slot_index = world_renderer.inventory_slot_at(mouse_x, mouse_y);
            if (slot_index.has_value()) {
                cmd_queue.push(std::make_unique<EquipCommandClient>(slot_index.value()));
                return;
            }
        }
        handle_left_click(event.button.x, event.button.y);
    }

    if (event.type == SDL_MOUSEWHEEL) {
        int mouse_x, mouse_y;
        SDL_GetMouseState(&mouse_x, &mouse_y);
        if (world_renderer.is_point_inside_console(mouse_x, mouse_y)) {
            world_renderer.scroll_console(-event.wheel.y);
        }
    }
}

void Client::handle_left_click(uint32_t mouse_x, uint32_t mouse_y) {
    const auto inv_slot = world_renderer.inventory_slot_at(mouse_x, mouse_y);
    if (inv_slot.has_value()) {
        selected_inv_slot = inv_slot;
        world_renderer.set_selected_inv_slot(inv_slot);
        return;
    }

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
            auto hit = world_renderer.get_entity_at_screen(static_cast<int>(mouse_x),
                                                           static_cast<int>(mouse_y));
            if (hit) {
                auto [entity_id, entity_type] = *hit;
                if (entity_type == EntityType::PLAYER || entity_type == EntityType::NPC) {
                    cmd_queue.push(std::make_unique<AttackCommandClient>(entity_id));
                } else if (entity_type == EntityType::CITIZEN) {
                    chat.select_npc(static_cast<int>(entity_id));
                    world_renderer.set_citizen_selected(static_cast<int>(entity_id));
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

void Client::handle_chat_input_event(const SDL_Event& event) {
    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RETURN && chat.is_active()) {
        std::string msg = chat.extract_message();
        if (!msg.empty()) {
            std::string lower = to_lower(msg);
            if (lower == "/help") {
                process_help_command();
            } else {
                std::optional<uint32_t> npc_id;
                if (chat.has_npc_selection()) {
                    npc_id = static_cast<uint32_t>(chat.get_selected_npc_id());
                }
                cmd_queue.push(std::make_unique<ChatCommandClient>(msg, npc_id, selected_inv_slot));
                if (msg[0] != '/' && msg[0] != '@') {
                    world_renderer.set_chat_bubble_on_local(msg);
                }
            }
        }
        chat.set_active(false);
        chat.clear_npc_selection();
        world_renderer.set_citizen_selected(-1);
        sync_chat_ui();
    }

    if (chat.is_active() && event.type == SDL_TEXTINPUT) {
        chat.append_text(event.text.text);
        sync_chat_ui();
    }

    if (chat.is_active() && event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_BACKSPACE) {
        chat.remove_last_char();
        sync_chat_ui();
    }
}

void Client::handle_events() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        handle_keyboard_event(event);
        handle_mouse_event(event);
        handle_chat_input_event(event);
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

// --- UI --- //

void Client::sync_chat_ui() {
    world_renderer.update_chat_input(chat.get_buffer(), chat.is_active());
}

void Client::process_help_command() {
    world_renderer.add_chat_message("--- Comandos disponibles ---", COLOR_YELLOW);
    world_renderer.add_chat_message("/comprar <item> - Comprar item a comerciante o sacerdote",
                                    COLOR_WHITE);
    world_renderer.add_chat_message("/vender <item> - Vender item al comerciante", COLOR_WHITE);
    world_renderer.add_chat_message("/listar - Ver catalogo de comerciante, banquero o sacerdote",
                                    COLOR_WHITE);
    world_renderer.add_chat_message("/depositar oro <cantidad> - Depositar oro en banco",
                                    COLOR_WHITE);
    world_renderer.add_chat_message("/depositar <item> - Depositar item en banco", COLOR_WHITE);
    world_renderer.add_chat_message("/retirar oro <cantidad> - Retirar oro del banco", COLOR_WHITE);
    world_renderer.add_chat_message("/retirar <item> - Retirar item del banco", COLOR_WHITE);
    world_renderer.add_chat_message("/curar - Curar via sacerdote", COLOR_WHITE);
    world_renderer.add_chat_message("/resucitar - Resucitar via sacerdote", COLOR_WHITE);
    world_renderer.add_chat_message("/meditar - Meditar para recuperar mana", COLOR_WHITE);
    world_renderer.add_chat_message("@<nick> <msg> - Mensaje privado a otro jugador", COLOR_WHITE);
    world_renderer.add_chat_message("/fundar-clan <nombre> - Fundar un clan", COLOR_WHITE);
    world_renderer.add_chat_message("/unirse <nombre> - Pide unirse a un clan", COLOR_WHITE);
    world_renderer.add_chat_message("/revisar-clan - Revisa pedidos pendientes y miembros",
                                    COLOR_WHITE);
    world_renderer.add_chat_message("/clan-aceptar <nick> - Acepta a un jugador al clan",
                                    COLOR_WHITE);
    world_renderer.add_chat_message("/clan-rechazar <nick> - Rechaza a un jugador del clan",
                                    COLOR_WHITE);
    world_renderer.add_chat_message("/clan-ban <nick> - Banea a un jugador del clan", COLOR_WHITE);
    world_renderer.add_chat_message("/dejar-clan - Dejar tu clan actual", COLOR_WHITE);
    world_renderer.add_chat_message("/listar-clanes - Ver lista de clanes disponibles",
                                    COLOR_WHITE);
    world_renderer.add_chat_message("/clan-kick <nick> - Expulsa a un jugador del clan",
                                    COLOR_WHITE);
}

// --- Game loop --- //

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

uint32_t Client::sleep_and_calc_next_frame(const uint32_t frame_start) const {
    const uint32_t current_ticks = SDL_GetTicks();
    const uint32_t elapsed = current_ticks - frame_start;

    if (elapsed < static_cast<uint32_t>(FRAME_MS)) {
        SDL_Delay(FRAME_MS - elapsed);
        return frame_count + 1;
    }
    const uint32_t frames_passed = (elapsed / FRAME_MS);
    return frame_count + std::max(1u, frames_passed);
}

// --- Cleanup --- //

void Client::close() {
    SoundManager::cleanup();
    skt.shutdown(2);
    skt.close();
    events_queue.close();
    cmd_queue.close();
    receiver.stop();
    sender.stop();
    receiver.join();
    sender.join();
}

// --- Public --- //

void Client::launch(const std::string& user, const std::string& pass) {
    try {
        last_move_command_ticks = 0;
        if (!user.empty()) {
            handle_login_sequence(user, pass);
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
            frame_count = sleep_and_calc_next_frame(frame_start);
        }
        close();
    } catch (const std::exception& e) {
        close();
    }
}
