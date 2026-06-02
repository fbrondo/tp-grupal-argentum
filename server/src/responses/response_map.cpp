#include "server/includes/responses/response_map.h"

ResponseMap::ResponseMap(const Map& map) : map(map) {}

void ResponseMap::execute(ServerProtocol& protocol) {
    //protocol.sendMap(this->map);
}
