#include "server/includes/commands/command_signup.h"

SignupCommand::SignupCommand(Id id, const std::string& username, const std::string& password):
        Command(id), username(username), password(password) {}

SignupInfo SignupCommand::getSignupInfo() { return std::make_tuple(client_id, username, password); }

void SignupCommand::execute(World& /*world*/) {}
