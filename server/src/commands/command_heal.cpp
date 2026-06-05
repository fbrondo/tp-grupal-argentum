#include "server/includes/commands/command_heal.h"

HealCommand::HealCommand(Id id): Command(id) {}

void HealCommand::execute(Gameloop& gameloop) {
    gameloop.processPlayerHeal(this->getIdPlayer());
}