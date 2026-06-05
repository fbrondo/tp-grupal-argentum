#pragma once

#include "../world.h"

#include "command.h"

class DropItemCommand: public Command {
private:
    Id instance_id;

public:
    DropItemCommand(Id id, Id instance_id);
    void execute(Gameloop& gameloop) override;
};
