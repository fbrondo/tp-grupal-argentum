#include "../../includes/npc/npc.h"

NPC::NPC(Id id_, TypeNPC type_, std::string&& name_, Position&& pos_, GameFormulas& formulas_):
    Entity(id_, std::move(pos_), formulas_), 
    type_npc(type_),
    name(std::move(name_)) {}
