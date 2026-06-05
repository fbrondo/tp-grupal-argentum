#pragma once
#include "command.h"

class DepositItemCommand : public Command {
private:
    Id instance_id;

public:
    DepositItemCommand(Id id, Id instance_id);      
    void execute(Gameloop& gameloop) override;
};