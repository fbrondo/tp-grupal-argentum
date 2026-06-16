#include "server/includes/commands/command_sell_item.h"

#include "server/includes/gameloop.h"

SellItemCommand::SellItemCommand(Id id, Id npc_id, uint8_t type_item): Command(id) {
    this->npc_id = npc_id;
    this->type_item = static_cast<TypeItem>(type_item);
}

void SellItemCommand::execute(Gameloop& game) { game.processSellItem(client_id, npc_id, type_item); }
