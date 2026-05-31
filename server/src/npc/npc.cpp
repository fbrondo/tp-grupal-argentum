#include "../../includes/npc/npc.h"
NPC::NPC(TypeNPC type_, std::string&& name, Position&& pos_):
        pos(std::move(pos_)), type(type_), name(std::move(name)) {}

const Position& NPC::getCurrentPosition() { this->pos; }
