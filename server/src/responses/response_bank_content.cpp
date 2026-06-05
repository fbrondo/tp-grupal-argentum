#include "server/includes/responses/response_bank_content.h"

ResponseBankContent::ResponseBankContent(std::vector<MsgItemInfo>&& items, uint32_t gold) : 
    bank_items(std::move(items)), gold_amount(gold) {}

void ResponseBankContent::execute(ServerProtocol& protocol) {
    protocol.sendBankContent(this->bank_items, this->gold_amount);
}