#pragma once

#include "client/includes/commands/command_client.h"

class InteractCommandClient: public ClientCommand {
private:
    uint32_t npc_id;
    uint8_t action;

public:
    InteractCommandClient(uint32_t npc_id, uint8_t action);
    void execute(ClientProtocol& protocol) const override;
};
