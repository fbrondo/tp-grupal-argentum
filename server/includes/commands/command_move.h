#pragma once

#include "../definitions.h"
#include "../world.h"
#include "common/includes/direction.h"

#include "command.h"


class MoveCommand: public Command {
private:
    uint8_t direction;

public:
    MoveCommand(uint32_t id, uint8_t dir);
    void execute(World& world) override;
    MoveInfo getMoveInfo();
};
