#include "server/includes/commands/command_deposit_gold.h"

#include "server/includes/gameloop.h"

DepositGoldCommand::DepositGoldCommand(Id id_, Id npc_id, uint32_t amount):
        Command(id_), amount(amount), npc_id(npc_id) {}

void DepositGoldCommand::execute(Gameloop& game) {
    game.processPlayerDepositGold(this->client_id, this->npc_id, this->amount);
}
