#include "../../includes/npc/banker.h"

Banker::Banker(Id id, TypeNPC type, std::string&& name, Position&& pos, GameFormulas& formulas) :
        CityNPC(id, type, std::move(name), std::move(pos), formulas) {}

// void Banker::interact(const Id& id_player, World& word, Comand& cmd) {}
