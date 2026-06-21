#pragma once
#include <string>

#include "server/includes/commands/command.h"

class ClanFoundCommand: public Command {
    std::string clan_name;

public:
    ClanFoundCommand(Id id, std::string clan_name);
    void execute(Gameloop& gameloop) override;
};
