#pragma once
#include "response.h"
#include "server_protocol.h"
#include <string>
#include <vector>

class ResponseTraderCatalog : public Response {
private:
    std::map<TypeItem, uint32_t> catalog;
public:
    explicit ResponseTraderCatalog(const std::map<TypeItem, std::unique_ptr<Item>>& store);
    void execute(ServerProtocol& protocol) override;
};