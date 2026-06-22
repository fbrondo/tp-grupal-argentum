#pragma once
#include <map>
#include <utility>

#include "common/includes/types.h"
#include "server/includes/responses/response.h"

class ResponseTraderCatalog: public Response {
private:
    std::map<TypeItem, std::pair<uint32_t, uint32_t>> catalog;


public:
    explicit ResponseTraderCatalog(std::map<TypeItem, std::pair<uint32_t, uint32_t>>&& catalog);
    void execute(ServerProtocol& protocol) override;
};
