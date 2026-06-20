#include "server/includes/commands/command_clan_kick.h"

#include "server/includes/gameloop.h"

ClanKickCommand::ClanKickCommand(Id id, std::string nick): Command(id), nick(std::move(nick)) {}

void ClanKickCommand::execute(Gameloop& gameloop) {
    gameloop.processClanKick(this->getIdPlayer(), this->nick);
}
