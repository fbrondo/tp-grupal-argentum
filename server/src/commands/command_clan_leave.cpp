#include "server/includes/commands/command_clan_leave.h"

#include "server/includes/gameloop.h"

ClanLeaveCommand::ClanLeaveCommand(Id id): Command(id) {}

void ClanLeaveCommand::execute(Gameloop& gameloop) {
    gameloop.processClanLeave(this->getIdPlayer());
}
