#include "../../includes/commands/command_login.h"

LoginCommand::LoginCommand(Id id, const std::string& name, const std::string& pass):
        Command(id) {
    this->username = name;
    this->password = pass;
}

/*void LoginCommand::execute(World& world) {
    //world.addPlayer(this->client_id);
}*/
