#include "../../includes/npc/merchant.h"

Merchant::Merchant(Id id, TypeNPC type, std::string&& name, Position&& pos,
                   std::map<TypeItem, std::unique_ptr<Item>>&& store, GameFormulas& formulas) :
        CityNPC(id, type, std::move(name), std::move(pos), formulas),
        store(std::move(store)) {
}

// void Merchant::interact(const Id& id_player, World& word, Comand& cmd) {}
