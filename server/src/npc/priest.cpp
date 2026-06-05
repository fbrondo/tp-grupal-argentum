#include "server/includes/npc/priest.h"

Priest::Priest(TypeNPC type, std::string&& name, Pose&& pose,
               std::map<TypeItem, std::unique_ptr<Item>>&& store_) :
        TraderNPC(type, std::move(name), std::move(pose), std::move(store_)) {}
// void Priest::interact(const Id& id_player, World& world, Comand& cmd) {}
