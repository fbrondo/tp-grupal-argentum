#pragma once
#include "command.h"

class EquipCommand: public Command {
private:
    Id instance_id;

public:
    EquipCommand(Id id, Id instance_id);
    void execute(Gameloop& gameloop) override;
};
