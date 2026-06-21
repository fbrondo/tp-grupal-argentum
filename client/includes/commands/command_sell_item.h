#pragma once

#include "client/includes/commands/command_client.h"

class SellItemCommandClient: public ClientCommand {
private:
    uint32_t npc_id;
    uint16_t item_id;
    uint16_t quantity;

public:
    SellItemCommandClient(uint32_t npc_id, uint16_t item_id, uint16_t quantity);
    void execute(ClientProtocol& protocol) const override;
};
