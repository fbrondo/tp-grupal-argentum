#include "server/includes/commands/command_attack.h"

AttackCommand::AttackCommand(Id player_id, Id target_id):
        Command(player_id), target_id(target_id) {}

void AttackCommand::execute(World& /*world*/) {
    // world.player_attack(this->client_id, this->target_id);
}

std::pair<Id, Id> AttackCommand::getAttackInfo() {
    return std::make_pair(this->client_id, this->target_id);
}
