#pragma once
#include <map>
#include <vector>

#include "common/includes/protocol.h"
#include "common/includes/types.h"
#include "server/includes/responses/response.h"

class ResponseBankContent: public Response {
private:
    std::map<TypeItem, uint32_t> bank_items;
    uint32_t gold_amount;

public:
    ResponseBankContent(std::map<TypeItem, uint32_t>&& items, uint32_t gold);
    void execute(ServerProtocol& protocol) override;
};
