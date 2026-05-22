#include "../includes/commands/command_move.h"

MoveCommand::MoveCommand(uint32_t id, uint8_t dir) : Command(id), direction(dir) {}

void MoveCommand::execute(World& world) {
    // world.move_player(this->client_id, this->direction);
}