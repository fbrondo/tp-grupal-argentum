#include "client/includes/commands/command_deposit_item.h"

DepositItemCommandClient::DepositItemCommandClient(uint16_t item_id): item_id(item_id) {}

void DepositItemCommandClient::execute(ClientProtocol& protocol) const {
    protocol.sendDepositItem(item_id);
}
