#pragma once

#include <string>

#include "client/includes/client.h"
#include "client/includes/commands/command_client.h"

class ChatCommandClient: public ClientCommand {
    std::string text;

public:
    explicit ChatCommandClient(std::string msg);
    void execute(ClientProtocol& protocol) const override;
};
