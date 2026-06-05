#include "server/includes/npc/trader.h"

TraderNPC::TraderNPC(TypeNPC type, std::string&& name, Pose&& pos,
                     std::map<TypeItem, std::unique_ptr<Item>>&& store) :
        CityNPC(type, std::move(name), std::move(pos)),
        store(std::move(store)) {}

const std::map<TypeItem, std::unique_ptr<Item>>& TraderNPC::getStore() const {
    return this->store;
}

bool TraderNPC::hasItem(TypeItem type) const {
    return this->store.find(type) != this->store.end();
}