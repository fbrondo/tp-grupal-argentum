#pragma once
#include <string>

#include "server/includes/commands/command.h"

class ClanBanCommand: public Command {
    std::string nick;

public:
    ClanBanCommand(Id id, std::string nick);
    void execute(Gameloop& gameloop) override;
};
