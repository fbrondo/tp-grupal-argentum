#include "server/includes/npc/merchant.h"

Merchant::Merchant(TypeNPC type, const std::string& name, const Pose& pose_,
                   std::map<TypeItem, Item*>&& items_):
        TraderNPC(type, name, pose_, std::move(items_)) {}

void Merchant::executePlayerSellsItem(Player& player, TypeItem type_item) const {
    if (!this->store.contains(type_item)) {
        return; /*SI NO ESTA EN SU STORE, NO COMPRA ESTE OBJETO*/
    }
    const auto item = dynamic_cast<ShopItem*>(this->store.at(type_item));
    player.sellItem(type_item, item->selling_price);
}
