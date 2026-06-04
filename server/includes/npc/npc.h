#ifndef NPC_H
#define NPC_H

#include <string>
#include <utility>

#include "common/includes/types.h"
#include "server/includes/core/map.h"
#include "server/includes/entity.h"

class NPC: public Entity {
private:
    TypeNPC type_npc;
    std::string name;

public:
    NPC(TypeNPC type, std::string&& name, Pose&& pos);
    virtual ~NPC() = default;
};

#endif
