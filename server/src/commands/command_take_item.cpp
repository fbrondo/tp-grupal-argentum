#include "../../includes/commands/command_take_item.h"

TakeItemCommand::TakeItemCommand(uint32_t id): Command(id) {}

void TakeItemCommand::execute(World& /*world*/) {
    // world.take_item_from_floor(this->client_id);
}
