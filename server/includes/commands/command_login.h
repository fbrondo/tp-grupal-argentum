#pragma once

#include <string>
#include "server/includes/commands/command.h"
#include "common/includes/types.h"
class World;


class LoginCommand: public Command {
private:
    std::string username;
    std::string password;

public:
    LoginCommand(const Id id, const std::string name, const std::string pass);
    void execute(World& world) override;
};
