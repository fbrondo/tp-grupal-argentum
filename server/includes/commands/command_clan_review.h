#pragma once
#include "server/includes/commands/command.h"

class ClanReviewCommand: public Command {
public:
    explicit ClanReviewCommand(Id id);
    void execute(Gameloop& gameloop) override;
};
