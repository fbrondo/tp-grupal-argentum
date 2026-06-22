#pragma once
#include "server/includes/commands/command.h"

class ClanListCommand: public Command {
public:
    explicit ClanListCommand(Id id);
    void execute(Gameloop& gameloop) override;
};
