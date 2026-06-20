#include "server/includes/commands/command_clan_reject.h"

#include "server/includes/gameloop.h"

ClanRejectCommand::ClanRejectCommand(Id id, std::string nick): Command(id), nick(std::move(nick)) {}

void ClanRejectCommand::execute(Gameloop& gameloop) {
    gameloop.processClanReject(this->getIdPlayer(), this->nick);
}
