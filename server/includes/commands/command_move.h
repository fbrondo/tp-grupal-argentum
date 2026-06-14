#pragma once

#include "server/includes/commands/command.h"

class MoveCommand: public Command {
private:
    uint8_t direction;

public:
    MoveCommand(Id id, uint8_t dir);
    void execute(Gameloop& game) override;
};
