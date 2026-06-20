#include "client/includes/commands/command_deposit_gold.h"

DepositGoldCommandClient::DepositGoldCommandClient(uint32_t amount): amount(amount) {}

void DepositGoldCommandClient::execute(ClientProtocol& protocol) const {
    protocol.sendDepositGold(amount);
}
