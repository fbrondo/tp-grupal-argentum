#include "../../includes/npc/criature.h"

Criature::Criature(TypeNPC type, std::string&& name, Position&& pos, uint16_t r_attack,
                   uint16_t hp_max, uint16_t level):
        NPC(type, std::move(name), std::move(pos)),
        current_hp(hp_max),
        range_attack(r_attack),
        hp_max_initial(hp_max),
        level(level) {}

void Criature::updatePosition(Position&& new_pos) { NPC::pos = std::move(new_pos); }
