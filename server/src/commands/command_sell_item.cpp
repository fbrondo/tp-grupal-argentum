#include "../../includes/commands/command_sell_item.h"

SellItemCommand::SellItemCommand(Id id, Id npc_id, Id item_id): Command(id) {
    this->npc_id = npc_id;
    this->item_id = item_id;
}

void SellItemCommand::execute(Gameloop& game) {
    game.process_sell_item(client_id, npc_id, item_id);
}
