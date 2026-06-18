#include "server/includes/npc/priest.h"

// Priest::Priest(TypeNPC type, std::string&& name, Pose&& pose,
//                std::map<TypeItem, std::unique_ptr<Item>>&& store_):
//         TraderNPC(type, std::move(name), std::move(pose), std::move(store_)) {}

Priest::Priest(TypeNPC type, const std::string& name, std::map<TypeItem, Item*>&& items_):
        TraderNPC(type, name, std::move(items_)) {}
