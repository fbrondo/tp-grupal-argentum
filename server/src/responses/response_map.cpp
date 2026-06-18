#include "server/includes/responses/response_map.h"

#include "server/includes/server_protocol.h"
ResponseMap::ResponseMap(Map&& map, std::vector<CitizenNpcSnapshot>&& citizen_):
        map(std::move(map)), citizen_npcs(std::move(citizen_)) {}

void ResponseMap::execute(ServerProtocol& protocol) {
    protocol.sendMap(this->map, this->citizen_npcs);
}
