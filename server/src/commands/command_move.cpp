#include "../../includes/commands/command_move.h"

MoveCommand::MoveCommand(uint32_t id, uint8_t dir): Command(id), direction(dir) {}


MoveInfo MoveCommand::getMoveInfo() {
    return std::make_tuple(client_id, static_cast<Direction>(this->direction));
}

void MoveCommand::execute(World& world) {
    world.movePlayer(this->client_id, static_cast<Direction>(this->direction));
}
