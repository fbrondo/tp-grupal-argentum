#pragma once
#include "common/includes/protocol.h"
#include "server/includes/responses/response.h"
#include "server/includes/server_protocol.h"

class ResponsePlayerStats: public Response {
private:
    MsgPlayerStats stats;

public:
    explicit ResponsePlayerStats(const MsgPlayerStats& stats);
    void execute(ServerProtocol& protocol) override;
};
