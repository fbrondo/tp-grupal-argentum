#pragma once
#include "command.h"

class CommandUnequip: public Command {
private:
    size_t slot_id;

public:
    CommandUnequip(Id id, size_t slot_id_);
    void execute(Gameloop& gameloop) override;
};
