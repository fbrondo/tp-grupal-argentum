#pragma once
#include "command.h"

class CommandUnequip: public Command {
private:
    Id instance_id;

public:
    CommandUnequip(Id id, Id instance_id);
    void execute(Gameloop& gameloop) override;
};
