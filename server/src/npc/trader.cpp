#include "server/includes/npc/trader.h"

TraderNPC::TraderNPC(TypeNPC type, const std::string& name, const Pose& pose_,
                     std::map<TypeItem, Item*>&& items_):
        CitizenNPC(type, name, pose_), store(std::move(items_)) {}

const std::map<TypeItem, Item*>& TraderNPC::getStore() const { return store; }

void TraderNPC::executeBuyItem(Player& player, TypeItem type_item_buy) {
    if (!this->store.contains(type_item_buy)) {
        return;
    }
    const auto item = dynamic_cast<ShopItem*>(this->store[type_item_buy]);
    player.buyItem(item);
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
