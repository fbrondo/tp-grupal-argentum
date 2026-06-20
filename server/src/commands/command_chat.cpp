#include "server/includes/commands/command_chat.h"

#include <sstream>

#include "server/includes/commands/command_heal.h"
#include "server/includes/commands/command_meditate.h"
#include "server/includes/commands/command_resurrect.h"
#include "server/includes/gameloop.h"

#define MEDITATE "/meditar"
#define HEAL "/curar"
#define RESURRECT "/resucitar"
#define DEBUG_KILL "/debug_morir"

ChatCommand::ChatCommand(Id id, std::string msg): Command(id), text(std::move(msg)) {}

void ChatCommand::execute(Gameloop& game) {
    if (this->text.empty())
        return;
    if (this->text[0] == '@') {
        auto space_pos = this->text.find(' ');
        if (space_pos != std::string::npos && space_pos > 1) {
            std::string nick = this->text.substr(1, space_pos - 1);
            std::string msg = this->text.substr(space_pos + 1);
            if (!msg.empty()) {
                game.processDirectChatByName(this->client_id, nick, msg);
            }
        }
        return;
    }
    if (this->text[0] == '/') {
        if (this->text == MEDITATE) {
            std::unique_ptr<Command> cmd = std::make_unique<MeditateCommand>(this->client_id);
            cmd->execute(game);
        } else if (this->text == HEAL) {
            std::unique_ptr<Command> cmd = std::make_unique<HealCommand>(this->client_id);
            cmd->execute(game);
        } else if (this->text == RESURRECT) {
            std::unique_ptr<Command> cmd = std::make_unique<ResurrectCommand>(this->client_id);
            cmd->execute(game);
        } else if (this->text == DEBUG_KILL) {
            game.processPlayerDebugKill(this->client_id);
        }
        return;
    }
    game.processBroadcastChat(this->client_id, this->text);
}
