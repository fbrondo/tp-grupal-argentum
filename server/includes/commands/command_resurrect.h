#pragma once
#include "server/includes/commands/command.h"

class ResurrectCommand: public Command {
public:
    explicit ResurrectCommand(uint32_t id);
    void execute(Gameloop& gameloop) override;
};
