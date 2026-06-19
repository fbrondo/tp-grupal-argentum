#pragma once

#include <cstdint>

#include "client/includes/commands/command_client.h"

class EquipCommandClient: public ClientCommand {
private:
    uint8_t slot_index;

public:
    explicit EquipCommandClient(uint8_t slot_index_);
    void execute(ClientProtocol& protocol) const override;
};
