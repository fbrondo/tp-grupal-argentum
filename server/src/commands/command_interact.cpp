#include "../includes/commands/command_interact.h"

InteractCommand::InteractCommand(uint32_t id, uint32_t npc_id): Command(id), npc_id(npc_id) {}

void InteractCommand::execute(World& world) {
    // world.interact_with_npc(client_id, npc_id);
}
