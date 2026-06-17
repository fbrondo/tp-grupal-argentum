#include "client/includes/commands/command_chat.h"

ChatCommandClient::ChatCommandClient(std::string msg): text(std::move(msg)) {}
void ChatCommandClient::execute(ClientProtocol& protocol) const { protocol.sendChat(text); }
