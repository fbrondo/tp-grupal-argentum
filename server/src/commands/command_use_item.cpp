#include "server/includes/commands/command_use_item.h"

#include "server/includes/gameloop.h"

UseItemCommand::UseItemCommand(Id id, Id instance_id): Command(id), instance_id(instance_id) {}

void UseItemCommand::execute(Gameloop& /*game*/) {
    // game.processPlayerUseItem(this->client_id, this->instance_id);
}
