#pragma once

#include "command.h"

class DropItemCommand: public Command {
private:
    uint8_t slot;

public:
    DropItemCommand(uint32_t id, uint8_t slot);
    void execute(World& world) override;
};
