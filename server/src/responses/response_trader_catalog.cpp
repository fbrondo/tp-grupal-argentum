#include "server/includes/responses/response_trader_catalog.h"

ResponseTraderCatalog::ResponseTraderCatalog(const std::map<TypeItem, std::unique_ptr<Item>>& store) {
    for (auto& [type, item] : store) {
        this->catalog[type] = item->selling_price; 
    }
}

void ResponseTraderCatalog::execute(ServerProtocol& protocol) {
    protocol.sendTraderCatalog(this->catalog);
}