#pragma once

#include "server/includes/commands/command.h"

class UseItemCommand: public Command {
private:
    Id instance_id;

public:
    UseItemCommand(Id id, Id instance_id);
    void execute(Gameloop& game) override;
};
