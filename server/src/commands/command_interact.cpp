#include "server/includes/commands/command_interact.h"

InteractCommand::InteractCommand(Id id, Id npc_id): Command(id), npc_id(npc_id) {}

void InteractCommand::execute(Gameloop& /*game world*/) {
    // world.interact_with_npc(client_id, npc_id);
}
