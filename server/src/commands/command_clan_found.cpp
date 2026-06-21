#include "server/includes/commands/command_clan_found.h"

#include "server/includes/gameloop.h"

ClanFoundCommand::ClanFoundCommand(Id id, std::string clan_name):
        Command(id), clan_name(std::move(clan_name)) {}

void ClanFoundCommand::execute(Gameloop& gameloop) {
    gameloop.processClanFound(this->getIdPlayer(), this->clan_name);
}
