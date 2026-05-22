#pragma once

#include "command.h"

class TakeItemCommand : public Command {
public:
    explicit TakeItemCommand(uint32_t id);
    void execute(World& world) override;
};