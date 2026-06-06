#include "server/includes/commands/command_login.h"
#include "server/includes/gameloop.h"

LoginCommand::LoginCommand(Id id, const std::string& name, const std::string& pass): Command(id), user(name,pass) {

}

void LoginCommand::execute(Gameloop& /*game*/) {}
