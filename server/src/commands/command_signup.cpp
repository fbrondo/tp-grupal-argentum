#include "server/includes/commands/command_signup.h"

#include "server/includes/gameloop.h"

SignupCommand::SignupCommand(Id id, const std::string& username, const std::string& pass,
                             CharacterTraits&& charact):
        Command(id), user(username, pass), charact(std::move(charact)) {}

void SignupCommand::execute(Gameloop& game) {
    game.processHandleSignup(client_id, this->user, this->charact);
}
