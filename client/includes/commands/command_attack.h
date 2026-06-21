#pragma once

#include <cstdint>

#include "client/includes/commands/command_client.h"

class AttackCommandClient: public ClientCommand {
private:
    uint32_t target_id;

public:
    explicit AttackCommandClient(uint32_t id);
    void execute(ClientProtocol& protocol) const override;
};
