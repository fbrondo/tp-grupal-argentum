#pragma once
#include "response.h"
#include "../server_protocol.h"
#include "common/includes/protocol.h"


class ResponseBankContent : public Response {
private:
    std::vector<MsgItemInfo> bank_items;
    uint32_t gold_amount;

public:
    ResponseBankContent(std::vector<MsgItemInfo>&& items, uint32_t gold);
    
    void execute(ServerProtocol& protocol) override;
};