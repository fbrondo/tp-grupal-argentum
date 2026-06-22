#pragma once

#include "client/includes/commands/command_client.h"

class DepositGoldCommandClient: public ClientCommand {
private:
    uint32_t npc_id;
    uint32_t amount;

public:
    explicit DepositGoldCommandClient(uint32_t npc_id, uint32_t amount);
    void execute(ClientProtocol& protocol) const override;
};
