#pragma once
#include "command_client.h"

class CreateCharacterCommandClient : public ClientCommand {
private:
    std::string username = "tomas";
    std::string password = "tomas123";
    uint8_t race = 1;
    uint8_t clase = 2;
public:
    explicit CreateCharacterCommandClient();
    void execute(ClientProtocol& protocol) const override;
};

