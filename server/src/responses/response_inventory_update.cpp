#include "server/includes/responses/response_inventory_update.h"
#include "server/includes/server_protocol.h"

ResponseInventoryUpdate::ResponseInventoryUpdate(const MsgInventoryUpdate& inv_data) : 
    inv(inv_data) {}

void ResponseInventoryUpdate::execute(ServerProtocol& protocol) {
    protocol.sendInventoryUpdate(this->inv);
}
