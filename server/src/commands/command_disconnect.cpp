#include "server/includes/commands/command_disconnect.h"

#include "server/includes/gameloop.h"

DisconnectCommand::DisconnectCommand(Id id): Command(id) {}

void DisconnectCommand::execute(Gameloop& game) { game.processDisconnect(this->client_id); }
