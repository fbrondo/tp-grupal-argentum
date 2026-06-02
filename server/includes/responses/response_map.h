#pragma once
#include "server/includes/responses/response.h"
#include "common/includes/map/map.h"

class ResponseMap : public Response {
private:
    const Map& map;
public:
    explicit ResponseMap(const Map& map);
    void execute(ServerProtocol& protocol) override;
};
