#pragma once
#include <vector>

#include "common/includes/core/snapshot.h"
#include "common/includes/map/map.h"
#include "server/includes/responses/response.h"
class ResponseMap: public Response {
private:
    Map map;
    std::vector<CitizenNpcSnapshot> citizen_npcs;

public:
    explicit ResponseMap(Map&& map, std::vector<CitizenNpcSnapshot>&& citizen_npcs_);
    void execute(ServerProtocol& protocol) override;
};
