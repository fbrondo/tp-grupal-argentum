#pragma once
#include "common/includes/map/map.h"
#include "server/includes/responses/response.h"
#include "server/includes/server_protocol.h"

class ResponseMap: public Response {
private:
    const Map& map;

public:
    explicit ResponseMap(Map&& map);
    void execute(ServerProtocol& protocol) override;
};
