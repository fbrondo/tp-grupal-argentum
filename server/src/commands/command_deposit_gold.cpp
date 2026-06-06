#include "server/includes/commands/command_deposit_gold.h"
#include "server/includes/gameloop.h"

DepositGoldCommand::DepositGoldCommand(Id id_, uint32_t amount): Command(id_), amount(amount) {}

void DepositGoldCommand::execute(Gameloop& game) {
    game.processPlayerDepositGold(this->client_id, amount);
}
