#pragma once

#include <cstddef>
#include <deque>
#include <string>

enum MessageColor { COLOR_WHITE, COLOR_GREEN, COLOR_RED, COLOR_YELLOW, COLOR_BLUE };

struct ParsedChatMessage {
    enum Type { PUBLIC, WHISPER_RECEIVED, WHISPER_SENT, SYSTEM };
    Type type;
    MessageColor color;
    std::string sender_name;
    std::string text;
};

class ChatManager {
private:
    std::string buffer;
    std::deque<std::string> log;
    bool active = false;
    int selected_npc_id = -1;

    static constexpr size_t MAX_LOG_SIZE = 6;
    static constexpr size_t MAX_MSG_LENGTH = 30;

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
