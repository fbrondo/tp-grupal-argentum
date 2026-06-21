#pragma once

#include "client/includes/commands/command_client.h"

class ListItemsCommandClient: public ClientCommand {
private:
    uint32_t npc_id;

public:
    explicit ListItemsCommandClient(uint32_t npc_id);
    void execute(ClientProtocol& protocol) const override;
};
