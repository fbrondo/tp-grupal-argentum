#pragma once
#include <cstdint>

#include "server/includes/responses/response.h"

class ResponseChangeMap: public Response {
private:
    uint16_t map_id;

public:
    explicit ResponseChangeMap(uint16_t map_id);
    void execute(ServerProtocol& protocol) override;
};
