#include "server/includes/commands/command_interact.h"

#include "server/includes/gameloop.h"

InteractCommand::InteractCommand(Id id, Id npc_id, uint8_t action):
        Command(id), npc_id(npc_id), action(action) {}

void InteractCommand::execute(Gameloop& game) {
    game.processPlayerInteract(this->client_id, this->npc_id, this->action);
}
