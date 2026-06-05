#pragma once

#include <string>
#include <tuple>

#include "common/includes/core/character_traits.h"
#include "common/includes/types.h"

#include "command.h"

using std::string;
using SignupInfo = std::tuple<Id, string, string, CharacterTraits>;

class SignupCommand: public Command {
private:
    string username;
    string password;
    CharacterTraits charact;


public:
    SignupCommand(Id id, const string& username, const string& pass, CharacterTraits&& charact);
    SignupInfo getSignupInfo();
    void execute(World& world) override;
};
