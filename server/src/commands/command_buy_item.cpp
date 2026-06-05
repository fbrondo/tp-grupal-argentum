#include "../../includes/commands/command_buy_item.h"

BuyItemCommand::BuyItemCommand(Id id, Id npc_id, Id item_id): Command(id) {
    this->npc_id = npc_id;
    this->item_id = item_id;
}

void BuyItemCommand::execute(Gameloop& game) { game.process_buy_item(client_id, npc_id, item_id); }
