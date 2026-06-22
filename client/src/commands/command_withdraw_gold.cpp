#include "client/includes/commands/command_withdraw_gold.h"

WithdrawGoldCommandClient::WithdrawGoldCommandClient(uint32_t npc_id, uint32_t amount):
        npc_id(npc_id), amount(amount) {}

void WithdrawGoldCommandClient::execute(ClientProtocol& protocol) const {
    protocol.sendWithdrawGold(npc_id, amount);
}
