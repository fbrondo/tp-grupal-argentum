#pragma once
#include <utility>

#include "common/includes/types.h"
#include "server/includes/commands/command.h"

class AttackCommand: public Command {
private:
    Id target_id;

public:
    AttackCommand(Id player_id, Id target_id);
    void execute(Gameloop& gameloop) override;
    // std::pair<Id, Id> getAttackInfo();
};
