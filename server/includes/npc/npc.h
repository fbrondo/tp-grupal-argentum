#ifndef NPC_H
#define NPC_H

#include <string>
#include <utility>

#include "../core/map.h"
#include "common/includes/types.h"

class NPC {

protected:
    const TypeNPC type;
    const std::string name;
    Position pos;

public:
    NPC(TypeNPC type_, std::string&& name, Position&& pos_):
            pos(std::move(pos_)), type(type_), name(std::move(name)) {}
    const Position& getCurrentPosition();
    virtual ~NPC() = default;
};

#endif
