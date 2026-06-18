#pragma once
#include "common/includes/protocol.h"
#include "server/includes/responses/response.h"

class ResponseEquipmentUpdate: public Response {
private:
    MsgEquipmentUpdate equip;

public:
    explicit ResponseEquipmentUpdate(const MsgEquipmentUpdate& equip_data);
    void execute(ServerProtocol& protocol) override;
};
