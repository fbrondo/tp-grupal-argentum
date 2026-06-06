#pragma once

#include "server/includes/commands/command.h"

class TakeItemCommand: public Command {
public:
    explicit TakeItemCommand(Id id);
    void execute(Gameloop& game) override;
};
