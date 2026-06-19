#pragma once

#include "server/includes/commands/command.h"

class DisconnectCommand: public Command {
public:
    explicit DisconnectCommand(Id id);
    void execute(Gameloop& game) override;
};
