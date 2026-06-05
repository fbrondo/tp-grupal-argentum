#include "server/includes/commands/command_deposit.h"

DepositItemCommand::DepositItemCommand(Id id, Id instance_id) : Command(id), instance_id(instance_id) {}

void DepositItemCommand::execute(Gameloop& gameloop) {
    gameloop.processPlayerDepositItem(this->client_id, this->instance_id);
}