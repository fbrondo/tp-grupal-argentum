#include "server/includes/commands/command_drop_item.h"

#include "server/includes/gameloop.h"

DropItemCommand::DropItemCommand(Id id, uint32_t slot_id): Command(id), index_slot(slot_id) {}

void DropItemCommand::execute(Gameloop& game) {
    game.processPlayerDropItem(client_id, this->index_slot);
}
