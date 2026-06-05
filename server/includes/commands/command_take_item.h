#pragma once

#include "../world.h"

#include "command.h"

class TakeItemCommand: public Command {
public:
    explicit TakeItemCommand(Id id);
    void execute(Gameloop& game) override;
};
