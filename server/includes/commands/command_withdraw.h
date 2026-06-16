#pragma once

#include "server/includes/commands/command.h"

class WithdrawItemCommand: public Command {
private:
    Id npc_id;
    TypeItem type_item;

public:
    WithdrawItemCommand(Id id, Id npc_id, uint8_t type_item);

    void execute(Gameloop& gameloop) override;
};
