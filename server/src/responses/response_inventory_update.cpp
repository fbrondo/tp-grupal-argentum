#include "server/includes/responses/response_inventory_update.h"

ResponseInventoryUpdate::ResponseInventoryUpdate(const MsgInventoryUpdate& inv): inv(inv) {}

void ResponseInventoryUpdate::execute(ServerProtocol& protocol) {
    protocol.sendInventoryUpdate(this->inv);
}
