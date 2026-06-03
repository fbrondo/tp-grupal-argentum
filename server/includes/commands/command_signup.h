#pragma once

#include <string>
#include <tuple>

#include "common/includes/types.h"

#include "command.h"

using SignupInfo = std::tuple<Id, std::string, std::string>;

class SignupCommand: public Command {
private:
    std::string username;
    std::string password;

public:
    SignupCommand(Id id, const std::string& username, const std::string& password);
    SignupInfo getSignupInfo();
    void execute(World& world) override;
};
