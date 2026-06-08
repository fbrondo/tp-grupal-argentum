#include "server/includes/responses/response_chat_msg.h"

#include "server/includes/server_protocol.h"

ResponseChatMsg::ResponseChatMsg(std::string message): message(std::move(message)) {}

void ResponseChatMsg::execute(ServerProtocol& protocol) { protocol.sendChatMsg(this->message); }
