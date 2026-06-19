#pragma once

#include <cstdint>

#include "client/includes/commands/command_client.h"
#include "common/includes/direction.h"

class MoveCommandClient: public ClientCommand {
private:
    Direction direction;

public:
    explicit MoveCommandClient(Direction dir);
    void execute(ClientProtocol& protocol) const override;
};
