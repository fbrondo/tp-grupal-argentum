#pragma once

#include "command.h"
#include "../world.h"

class DisconnectCommand: public Command {
public:
    explicit DisconnectCommand(uint32_t id);
    void execute(World& world) override;
};
