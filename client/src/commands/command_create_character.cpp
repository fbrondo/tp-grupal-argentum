#include "client/includes/commands/command_create_character.h"

CreateCharacterCommandClient::CreateCharacterCommandClient() {}

void CreateCharacterCommandClient::execute(ClientProtocol& protocol) const {
    protocol.sendRegister(username, password, race, clase);
}