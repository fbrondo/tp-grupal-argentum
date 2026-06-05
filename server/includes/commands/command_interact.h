#pragma once

#include "../world.h"

#include "command.h"


class InteractCommand: public Command {
    uint32_t npc_id;

public:
    InteractCommand(uint32_t id, uint32_t npc_id);
    void execute(Gameloop& game) override;
};
