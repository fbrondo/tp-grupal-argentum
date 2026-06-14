#include "server/includes/commands/command_use_item.h"

#include "server/includes/gameloop.h"

UseItemCommand::UseItemCommand(Id id, Id slot): Command(id), slot_id(slot) {}

void UseItemCommand::execute(Gameloop& /*world*/) {
    // world.use_item(client_id, slot);
}
