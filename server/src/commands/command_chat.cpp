#include "../../includes/commands/command_chat.h"

ChatCommand::ChatCommand(uint32_t id, std::string msg): Command(id), text(std::move(msg)) {}

void ChatCommand::execute(World& /*world*/) {
    // world.broadcast_chat_message(this->client_id, this->text);
}
