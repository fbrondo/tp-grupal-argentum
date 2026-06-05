#pragma once

#include "../world.h"

#include "command.h"

class SellItemCommand: public Command {
private:
    Id npc_id;
    Id item_id;

public:
    SellItemCommand(Id id, Id npc_id, Id instance_id);
    void execute(Gameloop& gameloop) override;
};
