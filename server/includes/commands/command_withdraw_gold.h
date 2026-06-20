#pragma once
#include "server/includes/commands/command.h"

class WithdrawGoldCommand: public Command {
private:
    uint32_t amount;
    Id npc_id;

public:
    explicit WithdrawGoldCommand(Id id_, Id npc_id, uint32_t amount);
    void execute(Gameloop& game) override;
};
