#pragma once

#include "../definitions.h"
#include "../world.h"
#include "common/includes/direction.h"

#include "command.h"


class MoveCommand: public Command {
private:
    uint8_t direction;

public:
    MoveCommand(Id id, uint8_t dir);
    void execute(Gameloop& game) override;
    MoveInfo getMoveInfo();
};
