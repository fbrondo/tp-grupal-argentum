#include "client/includes/commands/command_create_character.h"

CreateCharacterCommandClient::CreateCharacterCommandClient(std::string name, uint8_t race,
                                                           uint8_t clase):
        name(std::move(name)), race(race), clase(clase) {}

void CreateCharacterCommandClient::execute(ClientProtocol& /*protocol*/) const {
    // protocol.sendCharacterCreate(name, race, clase);
}