#pragma once

#include <string>
#include <tuple>
#include "common/includes/core/character_traits.h"
#include "common/includes/core/user.h"
#include "common/includes/types.h"
#include "server/includes/commands/command.h"

using std::string;
class SignupCommand: public Command {
private:
    User user;
    CharacterTraits charact;

public:
    SignupCommand(Id id, const string& username, const string& pass, CharacterTraits&& charact);
    //SignupInfo getSignupInfo();
    void execute(Gameloop& game) override;
};
