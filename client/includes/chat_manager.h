#pragma once

#include <cstddef>
#include <deque>
#include <string>

#include "client/includes/core/constants.h"
#include "client/includes/core/parsed_chat_message.h"

class ChatManager {
private:
    std::string buffer;
    std::deque<std::string> log;
    bool active = false;
    int selected_npc_id = -1;

public:
    ChatManager() = default;
    ~ChatManager() = default;

    bool is_active() const;
    const std::string& get_buffer() const;
    const std::deque<std::string>& get_log() const;

    void set_active(bool state);

    void append_text(const char* text);
    void remove_last_char();
    std::string extract_message();

    void select_npc(int server_id);
    void clear_npc_selection();
    bool has_npc_selection() const;
    int get_selected_npc_id() const;

    void add_message_to_log(const std::string& msg);
    static ParsedChatMessage parse_server_message(const std::string& payload);
};
