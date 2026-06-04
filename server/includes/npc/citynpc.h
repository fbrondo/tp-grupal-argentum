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
    CityNPC(TypeNPC type, std::string&& name, Pose&& pos):
            NPC(type, std::move(name), std::move(pos)) {}
    virtual ~CityNPC() = default;
    // virtual void interact(const Id& id_player, World& word, Comand& cmd) = 0;
};

#endif
