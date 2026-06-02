#include "server/includes/responses/response_change_map.h"

ResponseChangeMap::ResponseChangeMap(uint16_t map_id): map_id(map_id) {}

void ResponseChangeMap::execute(ServerProtocol& protocol) { protocol.sendChangeMap(this->map_id); }
