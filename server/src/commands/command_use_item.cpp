#include "server/includes/commands/command_use_item.h"

#include "server/includes/gameloop.h"

UseItemCommand::UseItemCommand(Id id, size_t slot_id): Command(id), slot_id(slot_id) {}

void UseItemCommand::execute(Gameloop& /*game*/) {
    // game.processPlayerUseItem(this->client_id, this->slot_id);
}
