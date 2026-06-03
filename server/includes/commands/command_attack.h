#pragma once
#include "../world.h"
#include "server/includes/commands/command.h"

class AttackCommand: public Command {
private:
    uint32_t target_id;

public:
    AttackCommand(uint32_t id, uint32_t target_id);
    void execute(World& world) override;
};
