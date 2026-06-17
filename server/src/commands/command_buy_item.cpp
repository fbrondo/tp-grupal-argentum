#include "server/includes/commands/command_buy_item.h"

#include "server/includes/gameloop.h"

BuyItemCommand::BuyItemCommand(Id id, Id npc_id, uint8_t type_item_): Command(id) {
    this->npc_id = npc_id;
    this->type_item = static_cast<TypeItem>(type_item_);
}

void BuyItemCommand::execute(Gameloop& game) {
    game.processBuyItem(client_id, this->npc_id, this->type_item);
}
