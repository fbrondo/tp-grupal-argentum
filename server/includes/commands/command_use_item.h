#pragma once

#include "server/includes/commands/command.h"

class UseItemCommand: public Command {
private:
    size_t slot_id;

public:
    UseItemCommand(Id id, size_t slot_id);
    void execute(Gameloop& game) override;
};
