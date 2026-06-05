#pragma once

#include "../world.h"

#include "command.h"

class BuyItemCommand: public Command {
private:
    Id npc_id;
    Id item_id;

public:
    BuyItemCommand(Id id, Id npc_id, Id item_id);
    void execute(World& world) override;
};
