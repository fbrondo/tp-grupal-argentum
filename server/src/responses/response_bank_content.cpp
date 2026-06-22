#include "server/includes/responses/response_bank_content.h"

#include "server/includes/server_protocol.h"

ResponseBankContent::ResponseBankContent(std::map<TypeItem, uint32_t>&& items, uint32_t gold):
        bank_items(std::move(items)), gold_amount(gold) {}

void ResponseBankContent::execute(ServerProtocol& protocol) {
    protocol.sendBankContent(this->bank_items, this->gold_amount);
}
