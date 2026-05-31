#pragma once

#include "command.h"
#include "../world.h"

class TakeItemCommand: public Command {
public:
    explicit TakeItemCommand(uint32_t id);
    void execute(World& world) override;
};
