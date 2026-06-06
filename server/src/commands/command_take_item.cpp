#include "server/includes/commands/command_take_item.h"
#include "server/includes/gameloop.h"

TakeItemCommand::TakeItemCommand(Id id): Command(id) {}

void TakeItemCommand::execute(Gameloop& game) { game.processPlayerPickUp(this->client_id); }
