#include "client/includes/commands/command_withdraw_item.h"

WithdrawItemCommandClient::WithdrawItemCommandClient(uint32_t npc_id, uint16_t item_id):
        npc_id(npc_id), item_id(item_id) {}

void WithdrawItemCommandClient::execute(ClientProtocol& protocol) const {
    protocol.sendWithdrawItem(npc_id, item_id);
}
