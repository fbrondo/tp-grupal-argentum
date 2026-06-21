#include "client/includes/commands/command_sell_item.h"

SellItemCommandClient::SellItemCommandClient(uint32_t npc_id, uint16_t item_id, uint16_t quantity):
        npc_id(npc_id), item_id(item_id), quantity(quantity) {}

void SellItemCommandClient::execute(ClientProtocol& protocol) const {
    protocol.sendSellItem(npc_id, item_id, quantity);
}
