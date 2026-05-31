#include "../../includes/npc/merchant.h"

Merchant::Merchant(TypeNPC type, std::string&& name, Position&& pos,
                   std::map<TypeItem, std::unique_ptr<Item>>&& store):
        CityNPC(type, std::move(name), std::move(pos)), store(std::move(store)) {}

//void Merchant::interact(const Id& id_player, World& word, Comand& cmd) {}
