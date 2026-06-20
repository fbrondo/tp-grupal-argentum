#pragma once
#include <map>

#include "common/includes/types.h"
#include "server/includes/responses/response.h"

class ResponseTraderCatalog: public Response {
private:
    std::map<TypeItem, uint32_t> catalog;


public:
    explicit ResponseTraderCatalog(std::map<TypeItem, uint32_t>&& catalog);
    void execute(ServerProtocol& protocol) override;
};
