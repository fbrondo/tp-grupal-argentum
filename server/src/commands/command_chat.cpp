#include "server/includes/commands/command_chat.h"

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
    // Chat común
    if (this->text.rfind("/", 0) != 0) {
        // game.processBroadcastChat(this->client_id, this->text); A implementar
        return;
    }
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
}
