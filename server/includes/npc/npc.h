#ifndef NPC_H
#define NPC_H

#include <string>
#include <utility>

#include "../core/map.h"
#include "../core/entity.h"
#include "common/includes/types.h"


class NPC: public Entity {
private:
    TypeNPC type_npc;
    std::string name;

public:
    NPC(Id id, TypeNPC type, std::string&& name, Position&& pos, GameFormulas& formulas);
    virtual ~NPC() = default;
};

#endif
