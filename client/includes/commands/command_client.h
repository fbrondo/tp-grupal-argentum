#pragma once
#include "client/includes/client_protocol.h"

class ClientCommand {
public:
    ClientCommand() = default;
    virtual void execute(ClientProtocol& protocol) const = 0;
    virtual ~ClientCommand() = default;
};
