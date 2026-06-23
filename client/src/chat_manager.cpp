#include "client/includes/chat_manager.h"

#include <cstring>

#include <SDL2/SDL.h>

bool ChatManager::is_active() const { return active; }

const std::string& ChatManager::get_buffer() const { return buffer; }

const std::deque<std::string>& ChatManager::get_log() const { return log; }

void ChatManager::set_active(bool state) {
    active = state;
    if (active) {
        SDL_StartTextInput();
    } else {
        SDL_StopTextInput();
    }
}

void ChatManager::append_text(const char* text) {
    if (active && buffer.size() + std::strlen(text) <= MAX_MSG_LENGTH) {
        buffer += text;
    }
}

void ChatManager::remove_last_char() {
    if (active && !buffer.empty()) {
        buffer.pop_back();
    }
}

std::string ChatManager::extract_message() {
    std::string msg = buffer;
    buffer.clear();
    return msg;
}

void ChatManager::select_npc(int server_id) { selected_npc_id = server_id; }

void ChatManager::clear_npc_selection() { selected_npc_id = -1; }

bool ChatManager::has_npc_selection() const { return selected_npc_id >= 0; }

int ChatManager::get_selected_npc_id() const { return selected_npc_id; }

void ChatManager::add_message_to_log(const std::string& msg) {
    log.push_back(msg);
    if (log.size() > MAX_LOG_SIZE) {
        log.pop_front();
    }
}

ParsedChatMessage ChatManager::parse_server_message(const std::string& payload) {
    using T = ParsedChatMessage::Type;
    const auto colon = payload.find(": ");
    if (colon != std::string::npos) {
        return {T::PUBLIC, COLOR_WHITE, payload.substr(0, colon), payload.substr(colon + 2)};
    }

    MessageColor color = COLOR_WHITE;
    if (payload.find("---") == 0) {
        color = COLOR_YELLOW;
    } else if (payload.find("Infligiste ") != std::string::npos ||
               payload.find("ha muerto") != std::string::npos) {
        color = COLOR_GREEN;
    } else if (payload.find("Recibiste ") != std::string::npos) {
        color = COLOR_RED;
    } else if (payload.find("esquiv") != std::string::npos) {
        color = COLOR_YELLOW;
    }
    return {T::SYSTEM, color, "", payload};
}
