#include "../includes/commands/command_sell_item.h"

SellItemCommand::SellItemCommand(uint32_t id, uint32_t npc_id, uint16_t item_id, uint16_t quantity):
        Command(id) {
    this->npc_id = npc_id;
    this->item_id = item_id;
    this->quantity = quantity;
}

void SellItemCommand::execute(World& world) {
    // world.sell_item(client_id, npc_id, item_id, quantity);
}
