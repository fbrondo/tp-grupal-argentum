#include "server/includes/commands/command_signup.h"

SignupCommand::SignupCommand(Id id, const string& username, const string& pass,
                             CharacterTraits&& charact):
        Command(id), username(username), password(pass), charact(std::move(charact)) {}

SignupInfo SignupCommand::getSignupInfo() {
    return std::make_tuple(this->client_id, this->username, this->password, this->charact);
}

void SignupCommand::execute(World& /*world*/) {}
