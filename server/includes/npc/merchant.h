#ifndef MERCHANT_H
#define MERCHANT_H

#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "common/includes/types.h"
#include "server/includes/core/item.h"
#include "server/includes/core/map.h"
#include "server/includes/npc/trader.h"
#include "server/includes/player.h"

class Merchant: public TraderNPC {
public:
    Merchant(TypeNPC type, const std::string& name, const Pose& pose_,
             std::map<TypeItem, Item*>&& items_);
    ~Merchant() override = default;
    void executePlayerSellsItem(Player& player, TypeItem type_item) const;
};

#endif
