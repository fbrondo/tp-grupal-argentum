#pragma once

#include "../world.h"

#include "command.h"

class DisconnectCommand: public Command {
public:
    explicit DisconnectCommand(Id id);
    void execute(Gameloop& game) override;
};
