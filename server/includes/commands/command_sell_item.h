#pragma once

#include "command.h"
#include "../world.h"

class SellItemCommand: public Command {
private:
    uint32_t npc_id;
    uint16_t item_id;
    uint16_t quantity;

public:
    SellItemCommand(uint32_t id, uint32_t npc_id, uint16_t item_id, uint16_t quantity);
    void execute(World& world) override;
};
