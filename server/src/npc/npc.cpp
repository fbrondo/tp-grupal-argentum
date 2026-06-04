#include "server/includes/npc/npc.h"

NPC::NPC(TypeNPC type_, std::string&& name_, Pose&& pose_):
        Entity(std::move(pose_)), type_npc(type_), name(std::move(name_)) {}
