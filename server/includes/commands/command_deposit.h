#pragma once
#include "server/includes/commands/command.h"

class DepositItemCommand: public Command {
private:
    Id npc_id;
    TypeItem type_item;

public:
    DepositItemCommand(Id id, Id npc_id, uint8_t type_item);
    void execute(Gameloop& gameloop) override;
};
