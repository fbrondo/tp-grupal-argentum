#pragma once

#include "client/includes/commands/command_client.h"

class WithdrawItemCommandClient: public ClientCommand {
private:
    uint16_t item_id;

public:
    explicit WithdrawItemCommandClient(uint16_t item_id);
    void execute(ClientProtocol& protocol) const override;
};
