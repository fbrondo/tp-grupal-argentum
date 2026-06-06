#include "server/includes/responses/response_inventory_update.h"
#include "server/includes/server_protocol.h"

ResponseInventoryUpdate::ResponseInventoryUpdate(const MsgInventoryUpdate& inv): inv(inv) {}

void ResponseInventoryUpdate::execute(ServerProtocol& protocol) {
    protocol.sendInventoryUpdate(this->inv);
}
