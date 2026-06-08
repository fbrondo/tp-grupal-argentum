#include "server/includes/commands/command_meditate.h"

#include "server/includes/gameloop.h"

MeditateCommand::MeditateCommand(Id id): Command(id) {}

void MeditateCommand::execute(Gameloop& gameloop) {
    gameloop.processPlayerMeditate(this->client_id);
}
