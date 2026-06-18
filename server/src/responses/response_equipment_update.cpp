#include "server/includes/responses/response_equipment_update.h"

#include "server/includes/server_protocol.h"

ResponseEquipmentUpdate::ResponseEquipmentUpdate(const MsgEquipmentUpdate& equip_data):
        equip(equip_data) {}

void ResponseEquipmentUpdate::execute(ServerProtocol& protocol) {
    protocol.sendEquipmentUpdate(this->equip);
}
