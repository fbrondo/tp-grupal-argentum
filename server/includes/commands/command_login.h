#pragma once

#include <string>
#include <tuple>

#include "common/includes/types.h"

#include "command.h"

using LoginInfo = std::tuple<Id, std::string, std::string>;

class LoginCommand: public Command {
private:
    std::string username;
    std::string password;

public:
    LoginCommand(Id id, const std::string& name, const std::string& pass);
    LoginInfo getLoginInfo();
    void execute(Gameloop& game) override;
};
