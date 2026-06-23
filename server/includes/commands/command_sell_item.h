#pragma once

#include "server/includes/commands/command.h"

class SellItemCommand: public Command {
private:
    Id npc_id;
    TypeItem type_item;

public:
    SellItemCommand(Id id, Id npc_id, uint8_t type_item);
    void execute(Gameloop& gameloop) override;
};
