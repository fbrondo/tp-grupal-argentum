#include "../includes/commands/command_disconnect.h"

DisconnectCommand::DisconnectCommand(uint32_t id): Command(id) {}

void DisconnectCommand::execute(World& world) {
    // world.disconnect_client(this->client_id);
}
