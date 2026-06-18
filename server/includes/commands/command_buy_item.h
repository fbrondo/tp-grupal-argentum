#pragma once

#include "server/includes/commands/command.h"

class BuyItemCommand: public Command {
private:
    Id npc_id;
    TypeItem type_item;

public:
    BuyItemCommand(Id id, Id npc_id, uint8_t type_item);
    void execute(Gameloop& game) override;
};
