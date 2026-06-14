#pragma once

#include <string>

#include "server/includes/commands/command.h"

class ChatCommand: public Command {
private:
    std::string text;

public:
    ChatCommand(Id id, std::string msg);
    void execute(Gameloop& game) override;
};
