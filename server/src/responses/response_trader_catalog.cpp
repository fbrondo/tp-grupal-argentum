#include "server/includes/responses/response_trader_catalog.h"

#include "server/includes/server_protocol.h"

ResponseTraderCatalog::ResponseTraderCatalog(std::map<TypeItem, uint32_t>&& catalog) {
    this->catalog = std::move(catalog);
}

void ResponseTraderCatalog::execute(ServerProtocol& protocol) {
    protocol.sendTraderCatalog(this->catalog);
}
