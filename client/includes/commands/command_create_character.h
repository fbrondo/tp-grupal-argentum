#pragma once

#include <cstdint>
#include <string>

#include "command_client.h"

class CreateCharacterCommandClient: public ClientCommand {
private:
    std::string name;
    uint8_t race;
    uint8_t clase;

public:
    CreateCharacterCommandClient(std::string name, uint8_t race, uint8_t clase);
    void execute(ClientProtocol& protocol) const override;
};
