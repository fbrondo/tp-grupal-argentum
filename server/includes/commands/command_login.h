#pragma once

#include <string>

#include "command.h"

class LoginCommand: public Command {
private:
    std::string username;
    std::string password;

public:
    LoginCommand(uint32_t id, const std::string& name, const std::string& pass);

    void execute(World& world) override;
};
