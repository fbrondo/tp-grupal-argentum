#include "server/includes/commands/command_withdraw.h"

WithdrawItemCommand::WithdrawItemCommand(Id id, Id instance_id) : Command(id), instance_id(instance_id) {}

void WithdrawItemCommand::execute(Gameloop& gameloop) {
    gameloop.processPlayerWithdrawItem(this->client_id, this->instance_id);
}