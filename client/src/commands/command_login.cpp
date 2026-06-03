#include "client/includes/commands/command_login.h"

LoginCommandClient::LoginCommandClient() {}

void LoginCommandClient::execute(ClientProtocol& protocol) const {
    protocol.sendLogin(username, password);
}
