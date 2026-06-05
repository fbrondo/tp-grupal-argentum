#include "server/includes/commands/command_login.h"

LoginCommand::LoginCommand(Id id, const std::string& name, const std::string& pass): Command(id) {
    this->username = name;
    this->password = pass;
}

LoginInfo LoginCommand::getLoginInfo() { return std::make_tuple(client_id, username, password); }

void LoginCommand::execute(Gameloop& game) {}
