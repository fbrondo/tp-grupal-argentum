#include "server/includes/npc/trader.h"

TraderNPC::TraderNPC(TypeNPC type, const std::string& name, const Pose& pose_,
                     std::map<TypeItem, Item*>&& items_):
        CitizenNPC(type, name, pose_), store(std::move(items_)) {}

bool TraderNPC::executeBuyItem(Player& player, TypeItem type_item_buy) {
    if (!this->store.contains(type_item_buy)) {
        return false;
    }
    const auto item = this->store[type_item_buy];
    return player.buyItem(item);
}

std::map<TypeItem, uint32_t> TraderNPC::listItemsCatalog() const {
    std::map<TypeItem, uint32_t> list;
    for (const auto [type, item]: this->store) {
        list.emplace(type, item->purchase_price);
    }
    return list;
}

// const std::map<TypeItem, std::unique_ptr<Item>>& TraderNPC::getStore() const { return
// this->store; }
//
// bool TraderNPC::hasItem(TypeItem type) const { return this->store.find(type) !=
// this->store.end(); }
//
// InteractionResult TraderNPC::interact() {
//     InteractionResult result;
//     result.type = InteractionType::TRADER_SHOP;
//     result.trader_store = &this->store;
//     return result;
// }
