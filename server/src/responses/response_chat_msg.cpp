#include "server/includes/responses/response_chat_msg.h"

ResponseChatMsg::ResponseChatMsg(std::string message) : message(std::move(message)) {}

void ResponseChatMsg::execute(ServerProtocol& protocol) {
    protocol.sendChatMsg(this->message);
}
