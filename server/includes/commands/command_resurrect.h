#pragma once

#include <optional>

#include "server/includes/commands/command.h"

class ResurrectCommand: public Command {
    std::optional<Id> priest_id;

public:
    explicit ResurrectCommand(Id id, std::optional<Id> priest_id = std::nullopt);
    void execute(Gameloop& gameloop) override;
};
