#include "client/includes/commands/command_deposit_item.h"

DepositItemCommandClient::DepositItemCommandClient(uint32_t npc_id, uint16_t item_id):
        npc_id(npc_id), item_id(item_id) {}

void DepositItemCommandClient::execute(ClientProtocol& protocol) const {
    protocol.sendDepositItem(npc_id, item_id);
}
