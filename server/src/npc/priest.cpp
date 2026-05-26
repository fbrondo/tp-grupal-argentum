#include "../../includes/npc/priest.h"

Priest::Priest(TypeNPC type, std::string&& name, Position&& pos,
               std::map<TypeItem, std::unique_ptr<Item>>&& store_):
        CityNPC(type, std::move(name), std::move(pos)), store(std::move(store_)) {}

void Priest::interact(const Id& id_player, World& world, Comand& cmd) {}
