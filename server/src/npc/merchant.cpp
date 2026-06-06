#include "server/includes/npc/merchant.h"

// Merchant::Merchant(TypeNPC type, std::string&& name, Pose&& pose,
//                    std::map<TypeItem, std::unique_ptr<Item>>&& store):
//         TraderNPC(type, std::move(name), std::move(pose), std::move(store)) {}

Merchant::Merchant(TypeNPC type, const std::string& name, Pose&& pose, std::vector<TypeItem> items_):
        TraderNPC(type, name, std::move(pose), items_) {}

// void Merchant::interact(const Id& id_player, World& word, Comand& cmd) {}
