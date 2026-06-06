#include "server/includes/commands/command_move.h"
#include "server/includes/gameloop.h"
#include "common/includes/direction.h"

MoveCommand::MoveCommand(Id id, uint8_t dir): Command(id), direction(dir) {}

void MoveCommand::execute(Gameloop& game) {
    game.processMovePlayer(this->client_id, static_cast<Direction>(this->direction));
}
