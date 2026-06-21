#include "client/includes/commands/command_buy_item.h"

BuyItemCommandClient::BuyItemCommandClient(uint32_t npc_id, uint16_t item_id, uint16_t quantity):
        npc_id(npc_id), item_id(item_id), quantity(quantity) {}

void BuyItemCommandClient::execute(ClientProtocol& protocol) const {
    protocol.sendBuyItem(npc_id, item_id, quantity);
}
