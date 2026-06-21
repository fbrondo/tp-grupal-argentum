#include "server/includes/commands/command_clan_accept.h"

#include "server/includes/gameloop.h"

ClanAcceptCommand::ClanAcceptCommand(Id id, std::string nick): Command(id), nick(std::move(nick)) {}

void ClanAcceptCommand::execute(Gameloop& gameloop) {
    gameloop.processClanAccept(this->getIdPlayer(), this->nick);
}
