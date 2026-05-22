#pragma once

#include "command.h"

class DisconnectCommand: public Command {
public:
    explicit DisconnectCommand(uint32_t id);
    void execute(World& world) override;
};
