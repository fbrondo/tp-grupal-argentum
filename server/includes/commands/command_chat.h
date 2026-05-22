#pragma once

#include "command.h"
#include <string>

class ChatCommand : public Command {
private:
    std::string text;
public:
    ChatCommand(uint32_t id, std::string msg);
    void execute(World& world) override;
};