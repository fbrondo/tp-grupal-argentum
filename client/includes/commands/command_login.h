#pragma once

#include <string>

#include "command_client.h"

class LoginCommandClient: public ClientCommand {
private:
    std::string username = "tomas";
    std::string password = "tomas123";

public:
    LoginCommandClient();
    void execute(ClientProtocol& protocol) const override;
};
