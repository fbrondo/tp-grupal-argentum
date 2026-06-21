#pragma once

#include "client/includes/commands/command_client.h"

class DepositItemCommandClient: public ClientCommand {
private:
    uint32_t npc_id;
    uint16_t item_id;

public:
    explicit DepositItemCommandClient(uint32_t npc_id, uint16_t item_id);
    void execute(ClientProtocol& protocol) const override;
};
