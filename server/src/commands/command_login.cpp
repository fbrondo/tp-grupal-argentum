#include "../includes/commands/command_login.h"

LoginCommand::LoginCommand(uint32_t id, std::string name, std::string pass): Command(id) {
    this->username = name;
    this->password = pass;
}

void LoginCommand::execute(World& world) {
    // world.login(this->client_id, this->username, this->password);
}
