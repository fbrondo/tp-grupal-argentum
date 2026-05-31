#pragma once

#include <string>
#include "command.h"
#include "../world.h"
#include "common/includes/types.h"

class LoginCommand: public Command {
private:
    std::string username;
    std::string password;

public:
    LoginCommand(Id id, const std::string& name, const std::string& pass);
    // void execute(World& world) override;
};
