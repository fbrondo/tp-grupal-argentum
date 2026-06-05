#include "server/includes/commands/command_withdraw_gold.h"

WithdrawGoldCommand::WithdrawGoldCommand(Id id_, uint32_t amount): Command(id_), amount(amount) {}

void WithdrawGoldCommand::execute(Gameloop& game) {
    game.processPlayerWithdrawGold(this->client_id, amount);
}