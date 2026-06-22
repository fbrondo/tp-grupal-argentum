#include "server/includes/commands/command_clan_list.h"

#include "server/includes/gameloop.h"

ClanListCommand::ClanListCommand(Id id): Command(id) {}

void ClanListCommand::execute(Gameloop& gameloop) { gameloop.processClanList(this->getIdPlayer()); }
