#include "server/includes/commands/command_resurrect.h"

#include "server/includes/gameloop.h"

ResurrectCommand::ResurrectCommand(Id id): Command(id) {}

void ResurrectCommand::execute(Gameloop& gameloop) { gameloop.processPlayerResurrect(client_id); }
