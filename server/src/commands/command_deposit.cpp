#include "server/includes/commands/command_deposit.h"

#include "server/includes/gameloop.h"

DepositItemCommand::DepositItemCommand(Id id, Id npc_id, uint8_t type_item):
        Command(id), npc_id(npc_id) {
    this->type_item = static_cast<TypeItem>(type_item);
}

void DepositItemCommand::execute(Gameloop& gameloop) {
    gameloop.processPlayerDepositItem(this->client_id, this->npc_id, this->type_item);
}
