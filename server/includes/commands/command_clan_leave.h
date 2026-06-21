#pragma once
#include "server/includes/commands/command.h"

class ClanLeaveCommand: public Command {
public:
    explicit ClanLeaveCommand(Id id);
    void execute(Gameloop& gameloop) override;
};
