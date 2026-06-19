#include "server/includes/commands/command_unequip.h"

#include "server/includes/gameloop.h"

CommandUnequip::CommandUnequip(Id id, size_t slot_id_): Command(id), slot_id(slot_id_) {}

void CommandUnequip::execute(Gameloop& gameloop) {
    gameloop.processPlayerUnequipItem(this->client_id, this->slot_id);
}
