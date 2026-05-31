#pragma once

#include "command.h"
#include "../world.h"

class UseItemCommand: public Command {
private:
    uint8_t slot;

public:
    UseItemCommand(uint32_t id, uint8_t slot);
    void execute(World& world) override;
};
