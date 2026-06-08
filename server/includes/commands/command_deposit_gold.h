#pragma once
#include "server/includes/commands/command.h"

class DepositGoldCommand: public Command {
private:
    uint32_t amount;

public:
    DepositGoldCommand(Id id_, uint32_t amount);
    void execute(Gameloop& game) override;
};
