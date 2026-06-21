#pragma once

#include "server/includes/commands/command.h"

class InteractCommand: public Command {
    Id npc_id;
    uint8_t action;

public:
    InteractCommand(Id id, Id npc_id, uint8_t action);
    void execute(Gameloop& game) override;
};
