#include "server/includes/responses/response_trader_catalog.h"

#include "server/includes/server_protocol.h"

ResponseTraderCatalog::ResponseTraderCatalog(const std::map<TypeItem, uint32_t>& catalog) {
    // for (auto& [type, item] : store) {
    //     this->catalog[type] = item->selling_price;
    // }
    this->catalog = catalog;
}

void ResponseTraderCatalog::execute(ServerProtocol& protocol) {
    protocol.sendTraderCatalog(this->catalog);
}
