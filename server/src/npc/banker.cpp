#include "../../includes/npc/banker.h"

Banker::Banker(TypeNPC type, std::string&& name, Position&& pos):
        CityNPC(type, std::move(name), std::move(pos)) {}

//void Banker::interact(const Id& id_player, World& word, Comand& cmd) {}
