#include "../includes/commands/command_attack.h"

AttackCommand::AttackCommand(uint32_t id, uint32_t target_id): Command(id), target_id(target_id) {}

void AttackCommand::execute(World& world) {
    // world.player_attack(this->client_id, this->target_id);
}
