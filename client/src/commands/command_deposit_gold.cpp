#include "client/includes/commands/command_deposit_gold.h"

DepositGoldCommandClient::DepositGoldCommandClient(uint32_t npc_id, uint32_t amount):
        npc_id(npc_id), amount(amount) {}

void DepositGoldCommandClient::execute(ClientProtocol& protocol) const {
    protocol.sendDepositGold(npc_id, amount);
}
