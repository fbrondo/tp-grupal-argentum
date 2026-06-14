#include "server/includes/commands/command_sell_item.h"

#include "server/includes/gameloop.h"

SellItemCommand::SellItemCommand(Id id, Id npc_id, Id item_id): Command(id) {
    this->npc_id = npc_id;
    this->item_id = item_id;
}

void SellItemCommand::execute(Gameloop& game) { game.processSellItem(client_id, npc_id, item_id); }
