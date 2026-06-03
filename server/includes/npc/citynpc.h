#ifndef CITYNPC_H
#define CITYNPC_H

#include "common/includes/types.h"
// #include "../commands/command.h"

#include <string>
#include <utility>

#include "../core/map.h"

#include "npc.h"

// class Comand;
// class World;

/*Representa un npc de ciudad/pueblo
    - Banquero
    - Sacerdote
    - Comerciante
*/
class CityNPC: public NPC {
public:
    CityNPC(Id id, TypeNPC type, std::string&& name, Position&& pos, GameFormulas& formulas) :
        NPC(id, type, std::move(name), std::move(pos), formulas) {}
    virtual ~CityNPC() = default;
    // virtual void interact(const Id& id_player, World& word, Comand& cmd) = 0;
};

#endif
