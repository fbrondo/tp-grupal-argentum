#include "../includes/commands/command_drop_item.h"

DropItemCommand::DropItemCommand(uint32_t id, uint8_t slot) : Command(id), slot(slot) {}

void DropItemCommand::execute(World& world) {
    // world.drop_item(client_id, slot);
}