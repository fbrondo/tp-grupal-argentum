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
    NPC(TypeNPC type_, std::string&& name, Position&& pos_);
    const Position& getCurrentPosition();
    virtual ~NPC() = default;
};

#endif
