#pragma once
#include <string>

#include "server/includes/commands/command.h"

class ClanKickCommand: public Command {
    std::string nick;

public:
    ClanKickCommand(Id id, std::string nick);
    void execute(Gameloop& gameloop) override;
};
