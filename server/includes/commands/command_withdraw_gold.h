#pragma once
#include "server/includes/commands/command.h"

class WithdrawGoldCommand: public Command {
private:
    uint32_t amount;

public:
    explicit WithdrawGoldCommand(Id id_, uint32_t amount);
    void execute(Gameloop& game) override;
};
