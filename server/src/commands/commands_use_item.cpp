#include "../includes/commands/command_use_item.h"

UseItemCommand::UseItemCommand(uint32_t id, uint8_t slot) : Command(id), slot(slot) {}

void UseItemCommand::execute(World& world) {
    // world.use_item(client_id, slot);
}