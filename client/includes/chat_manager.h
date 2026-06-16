#pragma once

#include <cstddef>  // Para size_t
#include <deque>
#include <string>

class ChatManager {
private:
    std::string buffer;
    std::deque<std::string> log;
    bool active = false;
    static constexpr size_t MAX_LOG_SIZE = 6;

public:
    ChatManager() = default;
    ~ChatManager() = default;

    // Getters
    bool is_active() const;
    const std::string& get_buffer() const;
    const std::deque<std::string>& get_log() const;

    // Control de estado
    void set_active(bool state);

    // Manejo de entrada
    void append_text(const char* text);
    void remove_last_char();

    // Extrae el mensaje actual y limpia el buffer para enviarlo
    std::string extract_message();

    // Agrega un mensaje nuevo al historial manteniendo el límite
    void add_message_to_log(const std::string& msg);
};
