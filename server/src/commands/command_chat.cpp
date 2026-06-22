#include "server/includes/commands/command_chat.h"

#include <cstring>

#include "server/includes/commands/command_clan_accept.h"
#include "server/includes/commands/command_clan_ban.h"
#include "server/includes/commands/command_clan_found.h"
#include "server/includes/commands/command_clan_join.h"
#include "server/includes/commands/command_clan_kick.h"
#include "server/includes/commands/command_clan_leave.h"
#include "server/includes/commands/command_clan_list.h"
#include "server/includes/commands/command_clan_reject.h"
#include "server/includes/commands/command_clan_review.h"
#include "server/includes/commands/command_meditate.h"
#include "server/includes/commands/command_resurrect.h"
#include "server/includes/gameloop.h"

#define MEDITATE "/meditar"
#define RESURRECT "/resucitar"
#define DEBUG_KILL "/debug_morir"
#define DEBUG_INFINITE_HP "/debug_vida_infinita"
#define DEBUG_INFINITE_MANA "/debug_mana_infinito"
#define CLAN_FOUND "/fundar-clan "
#define CLAN_JOIN "/unirse "
#define CLAN_REVIEW "/revisar-clan"
#define CLAN_ACCEPT "/clan-aceptar "
#define CLAN_REJECT "/clan-rechazar "
#define CLAN_BAN "/clan-ban "
#define CLAN_KICK "/clan-kick "
#define CLAN_LEAVE "/dejar-clan"
#define CLAN_LIST "/listar-clanes"

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
    if (this->text == MEDITATE) {
        std::unique_ptr<Command> cmd = std::make_unique<MeditateCommand>(this->client_id);
        cmd->execute(game);
        return;
    }
    if (this->text == RESURRECT) {
        std::unique_ptr<Command> cmd = std::make_unique<ResurrectCommand>(this->client_id);
        cmd->execute(game);
        return;
    }

    if (this->text == DEBUG_KILL) {
        game.processPlayerDebugKill(this->client_id);
        return;
    }
    if (this->text == DEBUG_INFINITE_HP) {
        game.processPlayerToggleInfiniteHp(this->client_id);
        return;
    }
    if (this->text == DEBUG_INFINITE_MANA) {
        game.processPlayerToggleInfiniteMana(this->client_id);
        return;
    }
    if (this->text.rfind(CLAN_FOUND, 0) == 0) {
        auto cmd = std::make_unique<ClanFoundCommand>(this->client_id,
                                                      this->text.substr(strlen(CLAN_FOUND)));
        cmd->execute(game);
        return;
    }
    if (this->text.rfind(CLAN_JOIN, 0) == 0) {
        auto cmd = std::make_unique<ClanJoinCommand>(this->client_id,
                                                     this->text.substr(strlen(CLAN_JOIN)));
        cmd->execute(game);
        return;
    }
    if (this->text == CLAN_REVIEW) {
        auto cmd = std::make_unique<ClanReviewCommand>(this->client_id);
        cmd->execute(game);
        return;
    }
    if (this->text.rfind(CLAN_ACCEPT, 0) == 0) {
        auto cmd = std::make_unique<ClanAcceptCommand>(this->client_id,
                                                       this->text.substr(strlen(CLAN_ACCEPT)));
        cmd->execute(game);
        return;
    }
    if (this->text.rfind(CLAN_REJECT, 0) == 0) {
        auto cmd = std::make_unique<ClanRejectCommand>(this->client_id,
                                                       this->text.substr(strlen(CLAN_REJECT)));
        cmd->execute(game);
        return;
    }
    if (this->text.rfind(CLAN_BAN, 0) == 0) {
        auto cmd = std::make_unique<ClanBanCommand>(this->client_id,
                                                    this->text.substr(strlen(CLAN_BAN)));
        cmd->execute(game);
        return;
    }
    if (this->text.rfind(CLAN_KICK, 0) == 0) {
        auto cmd = std::make_unique<ClanKickCommand>(this->client_id,
                                                     this->text.substr(strlen(CLAN_KICK)));
        cmd->execute(game);
        return;
    }
    if (this->text == CLAN_LEAVE) {
        auto cmd = std::make_unique<ClanLeaveCommand>(this->client_id);
        cmd->execute(game);
        return;
    }
    if (this->text == CLAN_LIST) {
        auto cmd = std::make_unique<ClanListCommand>(this->client_id);
        cmd->execute(game);
        return;
    }
    game.processBroadcastChat(this->client_id, this->text);
}
