#include "server/includes/commands/command_equip.h"

#include "server/includes/gameloop.h"

EquipCommand::EquipCommand(Id id, Id instance_id) : Command(id), instance_id(instance_id){}

void EquipCommand::execute(Gameloop& game) {
    game.processPlayerEquipItem(this->client_id, this->instance_id);
}