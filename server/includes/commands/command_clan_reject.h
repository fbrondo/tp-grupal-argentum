#pragma once
#include <string>

#include "server/includes/commands/command.h"

class ClanRejectCommand: public Command {
    std::string nick;

public:
    ClanRejectCommand(Id id, std::string nick);
    void execute(Gameloop& gameloop) override;
};
