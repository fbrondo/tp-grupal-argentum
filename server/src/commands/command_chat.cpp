#include "../../includes/commands/command_chat.h"

ChatCommand::ChatCommand(Id id, std::string msg): Command(id), text(std::move(msg)) {}

void ChatCommand::execute(Gameloop& game) {
    if (this->text.empty())
        return;

    // Chat común
    if (this->text.rfind("/", 0) != 0) {
        // game.processBroadcastChat(this->client_id, this->text); A implementar
        return;
    }

    if (this->text == "/meditar") {
        std::unique_ptr<Command> cmd = std::make_unique<MeditateCommand>(this->client_id);
        cmd->execute(game);
    } else if (this->text == "/curar") {
        std::unique_ptr<Command> cmd = std::make_unique<HealCommand>(this->client_id);
        cmd->execute(game);
    } else if (this->text == "/resucitar") {
        std::unique_ptr<Command> cmd = std::make_unique<ResurrectCommand>(this->client_id);
        cmd->execute(game);
    }
}
