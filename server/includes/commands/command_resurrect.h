#pragma once
#include "server/includes/commands/command.h"

class ResurrectCommand: public Command {
public:
    explicit ResurrectCommand(Id id);
    void execute(Gameloop& gameloop) override;
};
