#pragma once
#include <string>

#include "server/includes/commands/command.h"

class ClanAcceptCommand: public Command {
    std::string nick;

public:
    ClanAcceptCommand(Id id, std::string nick);
    void execute(Gameloop& gameloop) override;
};
