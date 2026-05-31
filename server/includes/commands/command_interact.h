#pragma once

#include "command.h"
#include "../world.h"


class InteractCommand: public Command {
    uint32_t npc_id;

public:
    InteractCommand(uint32_t id, uint32_t npc_id);
    void execute(World& world) override;
};
