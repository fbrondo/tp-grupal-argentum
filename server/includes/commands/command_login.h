#pragma once

#include <string>
#include "common/includes/core/user.h"
#include "server/includes/commands/command.h"

class LoginCommand: public Command {
private:
   User user;

public:
    LoginCommand(Id id, const std::string& name, const std::string& pass);
    void execute(Gameloop& game) override;
};
