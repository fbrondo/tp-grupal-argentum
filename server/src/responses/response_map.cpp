#include "server/includes/responses/response_map.h"

#include "server/includes/server_protocol.h"
ResponseMap::ResponseMap(Map&& map): map(std::move(map)) {}

void ResponseMap::execute(ServerProtocol& protocol) { protocol.sendMap(this->map); }
