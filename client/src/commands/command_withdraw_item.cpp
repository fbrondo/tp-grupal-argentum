#include "client/includes/commands/command_withdraw_item.h"

WithdrawItemCommandClient::WithdrawItemCommandClient(uint16_t item_id): item_id(item_id) {}

void WithdrawItemCommandClient::execute(ClientProtocol& protocol) const {
    protocol.sendWithdrawItem(item_id);
}
