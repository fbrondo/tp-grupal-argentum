#include "client/includes/commands/command_withdraw_gold.h"

WithdrawGoldCommandClient::WithdrawGoldCommandClient(uint32_t amount): amount(amount) {}

void WithdrawGoldCommandClient::execute(ClientProtocol& protocol) const {
    protocol.sendWithdrawGold(amount);
}
