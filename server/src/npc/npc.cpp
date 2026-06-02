#include "../../includes/npc/npc.h"
NPC::NPC(TypeNPC type_, std::string&& name, Position&& pos_):
        type(type_), name(std::move(name)), pos(std::move(pos_)) {}

const Position& NPC::getCurrentPosition() { return this->pos; }
