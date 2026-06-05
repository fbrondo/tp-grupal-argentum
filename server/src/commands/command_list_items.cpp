#include "server/includes/commands/command_list_items.h"

ListItemsCommand::ListItemsCommand(Id id, Id npc_id) : Command(id), npc_id(npc_id) {}

void ListItemsCommand::execute(Gameloop& gameloop) {
    gameloop.processListItems(this->getIdPlayer(), this->npc_id);
}