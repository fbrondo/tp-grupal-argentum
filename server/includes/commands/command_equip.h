#pragma once
#include "command.h"

class EquipCommand: public Command {
private:
    size_t slot_id;

public:
    EquipCommand(Id id, uint32_t slot_id);
    void execute(Gameloop& gameloop) override;
};
