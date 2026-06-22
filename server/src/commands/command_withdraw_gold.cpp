#include "server/includes/commands/command_withdraw_gold.h"

#include "server/includes/gameloop.h"

WithdrawGoldCommand::WithdrawGoldCommand(Id id_, Id npc_id, uint32_t amount):
        Command(id_), amount(amount), npc_id(npc_id) {}

void WithdrawGoldCommand::execute(Gameloop& game) {
    game.processPlayerWithdrawGold(this->client_id, this->npc_id, this->amount);
}
