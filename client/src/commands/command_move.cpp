#include "client/includes/commands/command_move.h"

MoveCommandClient::MoveCommandClient(const Direction dir): direction(dir) {}

void MoveCommandClient::execute(ClientProtocol& protocol) const {
    protocol.sendMove(direction);
}