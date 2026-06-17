#include "client/includes/chat_manager.h"

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
    if (active) {
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

void ChatManager::add_message_to_log(const std::string& msg) {
    log.push_back(msg);
    if (log.size() > MAX_LOG_SIZE) {
        log.pop_front();
    }
}
