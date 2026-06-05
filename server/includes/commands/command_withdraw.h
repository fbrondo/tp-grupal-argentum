#pragma once

#include "command.h"

class WithdrawItemCommand : public Command {
private:
    Id instance_id;

public:
    WithdrawItemCommand(Id id, Id instance_id);
            
    void execute(Gameloop& gameloop) override;
};