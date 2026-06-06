#pragma once
#include "common/includes/protocol.h"
#include "server/includes/responses/response.h"

class ResponseInventoryUpdate: public Response {
private:
    MsgInventoryUpdate inv;

public:
    explicit ResponseInventoryUpdate(const MsgInventoryUpdate& inv);
    void execute(ServerProtocol& protocol) override;
};
