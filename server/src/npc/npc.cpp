#include "server/includes/npc/npc.h"

NPC::NPC(TypeNPC type_, const std::string& name_, Pose&& pose_):
        Entity(std::move(pose_)), type_npc(type_), name(name_) {}

TypeNPC NPC::getTypeNPC() {
        return this->type_npc;
}
