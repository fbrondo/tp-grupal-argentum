#include "server/includes/commands/command_drop_item.h"
#include "server/includes/gameloop.h"

DropItemCommand::DropItemCommand(Id id, Id instance_id): Command(id), instance_id(instance_id) {}

void DropItemCommand::execute(Gameloop& game) {
    game.processPlayerDropItem(client_id, instance_id);
}
