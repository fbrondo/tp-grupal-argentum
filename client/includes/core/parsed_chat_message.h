#pragma once

#include <string>

#include "client/includes/core/constants.h"

struct ParsedChatMessage {
    enum Type { PUBLIC, WHISPER_RECEIVED, WHISPER_SENT, SYSTEM };
    Type type;
    MessageColor color;
    std::string sender_name;
    std::string text;
};
