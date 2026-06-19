#pragma once

#include <string>

#include "command_client.h"

class LoginCommandClient: public ClientCommand {
private:
    std::string username;
    std::string password;

public:
    LoginCommandClient();
    void execute(ClientProtocol& protocol) const override;
};
