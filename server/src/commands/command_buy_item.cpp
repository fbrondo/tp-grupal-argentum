#include "../../includes/commands/command_buy_item.h"

BuyItemCommand::BuyItemCommand(uint32_t id, uint32_t npc_id, uint16_t item_id):
        Command(id) {
    this->npc_id = npc_id;
    this->item_id = item_id;
}

void BuyItemCommand::execute(World& /*world*/) {
    // world.buy_item(client_id, npc_id, item_id, quantity);
}
