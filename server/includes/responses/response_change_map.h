#pragma once
#include "server/includes/responses/response.h"
#include "server/includes/server_protocol.h"
#include <cstdint>

class ResponseChangeMap : public Response {
private:
    uint16_t map_id;
public:
    explicit ResponseChangeMap(uint16_t map_id);
    void execute(ServerProtocol& protocol) override;
};
