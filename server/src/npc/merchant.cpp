#include "server/includes/npc/merchant.h"

#include "server/includes/exceptions/invalid_sell_exception.h"
#include "server/includes/game_message_builder.h"
Merchant::Merchant(TypeNPC type, const std::string& name, const Pose& pose_,
                   std::map<TypeItem, Item*>&& items_):
        TraderNPC(type, name, pose_, std::move(items_)) {}

void Merchant::executePlayerSellsItem(Player& player, TypeItem type_item) const {
    if (!this->store.contains(type_item)) {
        const auto mssg = GameMessageBuilder::messgThisNPCDoesNotbuyItem(this->name);
        throw InvalidSellException(mssg);
    }
    const auto item = this->store.at(type_item);
    player.sellItem(type_item, item->selling_price);
}
