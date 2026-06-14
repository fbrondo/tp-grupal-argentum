#pragma once

#include "server/includes/commands/command.h"

class DropItemCommand: public Command {
private:
    Id instance_id;

public:
    DropItemCommand(Id id, Id instance_id);
    void execute(Gameloop& gameloop) override;
};
