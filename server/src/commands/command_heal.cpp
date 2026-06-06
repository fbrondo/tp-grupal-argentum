#include "server/includes/commands/command_heal.h"
#include "server/includes/gameloop.h"

HealCommand::HealCommand(Id id): Command(id) {}

void HealCommand::execute(Gameloop& gameloop) {
    gameloop.processPlayerHeal(this->getIdPlayer());
}