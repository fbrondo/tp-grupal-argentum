#include "server/includes/commands/command_clan_join.h"

#include "server/includes/gameloop.h"

ClanJoinCommand::ClanJoinCommand(Id id, std::string clan_name):
        Command(id), clan_name(std::move(clan_name)) {}

void ClanJoinCommand::execute(Gameloop& gameloop) {
    gameloop.processClanJoin(this->getIdPlayer(), this->clan_name);
}
