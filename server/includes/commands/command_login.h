#pragma once

#include <string>

#include "common/includes/types.h"
#include "server/includes/commands/command.h"
class World;


class LoginCommand: public Command {
private:
    std::string username;
    std::string password;

public:
    LoginCommand(Id id, const std::string& name, const std::string& pass);
    void execute(World& world) override;
};
