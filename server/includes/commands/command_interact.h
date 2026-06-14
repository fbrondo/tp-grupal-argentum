#pragma once

#include "server/includes/commands/command.h"

class InteractCommand: public Command {
    Id npc_id;

public:
    InteractCommand(Id id, Id npc_id);
    void execute(Gameloop& game) override;
};
