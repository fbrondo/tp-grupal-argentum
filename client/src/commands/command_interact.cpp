#include "client/includes/commands/command_interact.h"

InteractCommandClient::InteractCommandClient(uint32_t npc_id, uint8_t action):
        npc_id(npc_id), action(action) {}

void InteractCommandClient::execute(ClientProtocol& protocol) const {
    protocol.sendInteract(npc_id, action);
}
