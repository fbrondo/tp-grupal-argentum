#include "server/includes/commands/command_equip.h"

#include "server/includes/gameloop.h"

EquipCommand::EquipCommand(Id id, uint32_t slot_id_): Command(id), slot_id(slot_id_) {}

void EquipCommand::execute(Gameloop& game) {
    game.processPlayerEquipItem(this->client_id, this->slot_id);
}
