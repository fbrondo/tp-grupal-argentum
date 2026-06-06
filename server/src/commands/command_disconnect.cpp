#include "server/includes/commands/command_disconnect.h"
#include "server/includes/gameloop.h"

DisconnectCommand::DisconnectCommand(uint32_t id): Command(id) {}

void DisconnectCommand::execute(Gameloop& /*game*/) {
    // world.disconnect_client(this->client_id);
}
