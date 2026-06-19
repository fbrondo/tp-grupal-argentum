#include "client/includes/commands/command_unequip.h"

UnequipCommandClient::UnequipCommandClient(uint8_t slot_index_): slot_index(slot_index_) {}

void UnequipCommandClient::execute(ClientProtocol& protocol) const {
    protocol.sendUnequipItem(slot_index);
}
