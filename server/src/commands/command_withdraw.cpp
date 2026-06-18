#include "server/includes/commands/command_withdraw.h"

#include "server/includes/gameloop.h"

WithdrawItemCommand::WithdrawItemCommand(Id id, Id npc_id, uint8_t type_item):
        Command(id), npc_id(npc_id) {
    this->type_item = static_cast<TypeItem>(type_item);
}

void WithdrawItemCommand::execute(Gameloop& gameloop) {
    gameloop.processPlayerWithdrawItem(this->client_id, this->npc_id, this->type_item);
}
