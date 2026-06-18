#pragma once

#include "server/includes/commands/command.h"

class DropItemCommand: public Command {
private:
    size_t index_slot;

public:
    DropItemCommand(Id id, uint32_t slot_id);
    void execute(Gameloop& gameloop) override;
};
