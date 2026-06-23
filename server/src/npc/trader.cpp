#include "server/includes/npc/trader.h"

#include "server/includes/exceptions/invalid_buy_exception.h"

TraderNPC::TraderNPC(const std::string& name, const NpcInstance& instance,
                     std::map<TypeItem, Item*>&& items_):
        CitizenNPC(instance.type, name, instance.pose, instance.zone_id),
        store(std::move(items_)) {}

void TraderNPC::executeBuyItem(Player& player, TypeItem type_item_buy) {
    if (!this->store.contains(type_item_buy)) {
        throw InvalidBuyException("Ese objeto no está en el catálogo.");
    }
    const auto item = this->store[type_item_buy];
    player.buyItem(item);
}

std::map<TypeItem, std::pair<uint32_t, uint32_t>> TraderNPC::listItemsCatalog() const {
    std::map<TypeItem, std::pair<uint32_t, uint32_t>> list;
    for (const auto [type, item]: this->store) {
        list.emplace(type, std::make_pair(item->purchase_price, item->selling_price));
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
