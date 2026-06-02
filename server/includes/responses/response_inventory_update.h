#pragma once
#include "server/includes/responses/response.h"
#include "server/includes/server_protocol.h"
#include "common/includes/protocol.h"

class ResponseInventoryUpdate : public Response {
private:
    MsgInventoryUpdate inv;
public:
    explicit ResponseInventoryUpdate(const MsgInventoryUpdate& inv);
    void execute(ServerProtocol& protocol) override;
};
