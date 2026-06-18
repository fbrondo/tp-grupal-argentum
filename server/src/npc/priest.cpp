#include "server/includes/npc/priest.h"

Priest::Priest(TypeNPC type, const std::string& name, const Pose& pose_,
               std::map<TypeItem, Item*>&& items_):
        TraderNPC(type, name, pose_, std::move(items_)) {}
