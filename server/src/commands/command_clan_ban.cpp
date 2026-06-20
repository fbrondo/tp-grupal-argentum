#include "server/includes/commands/command_clan_ban.h"

#include "server/includes/gameloop.h"

ClanBanCommand::ClanBanCommand(Id id, std::string nick): Command(id), nick(std::move(nick)) {}

void ClanBanCommand::execute(Gameloop& gameloop) {
    gameloop.processClanBan(this->getIdPlayer(), this->nick);
}
