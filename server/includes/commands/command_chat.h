#pragma once

#include <string>

#include "command.h"
#include "../world.h"

class ChatCommand: public Command {
private:
    std::string text;

public:
    ChatCommand(uint32_t id, std::string msg);
    void execute(World& world) override;
};
