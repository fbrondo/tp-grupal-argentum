#include "client/includes/commands/command_list_items.h"

ListItemsCommandClient::ListItemsCommandClient(uint32_t npc_id): npc_id(npc_id) {}

void ListItemsCommandClient::execute(ClientProtocol& protocol) const {
    protocol.sendListItems(npc_id);
}
