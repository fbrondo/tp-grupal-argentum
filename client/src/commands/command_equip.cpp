#include "client/includes/commands/command_equip.h"

EquipCommandClient::EquipCommandClient(uint8_t slot_index_): slot_index(slot_index_) {}

void EquipCommandClient::execute(ClientProtocol& protocol) const {
    protocol.sendEquipItem(slot_index);
}
