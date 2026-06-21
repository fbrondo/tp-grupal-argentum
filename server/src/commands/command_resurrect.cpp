#include "server/includes/commands/command_resurrect.h"

#include "server/includes/gameloop.h"

ResurrectCommand::ResurrectCommand(Id id, std::optional<Id> priest_id):
        Command(id), priest_id(priest_id) {}

void ResurrectCommand::execute(Gameloop& gameloop) {
    gameloop.processPlayerResurrect(this->getIdPlayer(), this->priest_id);
}
