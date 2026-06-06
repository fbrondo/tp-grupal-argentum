#pragma once
#include "server/includes/commands/command.h"

class MeditateCommand : public Command {
public:
    explicit MeditateCommand(Id id);
    void execute(Gameloop& gameloop) override;
};
