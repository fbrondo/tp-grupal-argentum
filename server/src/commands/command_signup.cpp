#include "server/includes/commands/command_signup.h"

SignupCommand::SignupCommand(Id id, const string& username, const string& pass, CharacterTraits&& charact):
        Command(id), user(username, pass), charact(std::move(charact)) {}

// SignupInfo SignupCommand::getSignupInfo() {
//     return std::make_tuple(client_id, this->username, this->password, this->charact);
// }

void SignupCommand::execute(Gameloop& game) {
    game.processHandleSignup(client_id, this->user, this->charact);
}
