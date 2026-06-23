#include "server/includes/commands/command_attack.h"

#include "server/includes/gameloop.h"

AttackCommand::AttackCommand(Id player_id, Id target_id):
        Command(player_id), target_id(target_id) {}

void AttackCommand::execute(Gameloop& game) {
    game.executeAttackPlayer(client_id, this->target_id);
}
