#pragma once
#include <string>

#include "server/includes/commands/command.h"

class ClanJoinCommand: public Command {
    std::string clan_name;

public:
    ClanJoinCommand(Id id, std::string clan_name);
    void execute(Gameloop& gameloop) override;
};
