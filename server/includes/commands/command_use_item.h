#pragma once

#include "server/includes/commands/command.h"

class UseItemCommand: public Command {
private:
    Id slot_id;

public:
    UseItemCommand(Id id, Id slot_id);
    void execute(Gameloop& game) override;
};
