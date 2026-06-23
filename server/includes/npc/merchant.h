#ifndef MERCHANT_H
#define MERCHANT_H

#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "common/includes/types.h"
#include "server/includes/core/item.h"
#include "server/includes/npc/trader.h"
#include "server/includes/player.h"

class Merchant: public TraderNPC {
public:
    Merchant(const std::string& name, const NpcInstance& instance,
             std::map<TypeItem, Item*>&& items_);
    ~Merchant() override = default;
    void executePlayerSellsItem(Player& player, TypeItem type_item) const;
};

#endif
