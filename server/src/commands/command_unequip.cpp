#include "server/includes/commands/command_unequip.h"

#include "server/includes/gameloop.h"

CommandUnequip::CommandUnequip(Id id, Id instance_id): Command(id), instance_id(instance_id) {}

void CommandUnequip::execute(Gameloop& /*gameloop*/) {
    // gameloop.processPlayerUnequipItem(this->client_id, this->instance_id);
}
