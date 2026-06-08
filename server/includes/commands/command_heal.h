#pragma once
#include "server/includes/commands/command.h"

class HealCommand: public Command {
public:
    explicit HealCommand(Id id);
    void execute(Gameloop& gameloop) override;
};
