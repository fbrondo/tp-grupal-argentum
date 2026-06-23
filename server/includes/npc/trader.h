#pragma once

#include <map>
#include <memory>
#include <string>
#include <utility>

#include "server/includes/core/item.h"
#include "server/includes/npc/citizen_npc.h"
#include "server/includes/player.h"

class TraderNPC: public CitizenNPC {
protected:
    std::map<TypeItem, Item*> store;

public:
    TraderNPC(const std::string& name, const NpcInstance& instance,
              std::map<TypeItem, Item*>&& items);
    ~TraderNPC() override = default;
    virtual void executeBuyItem(Player& player, TypeItem type_item_buy);
    virtual std::map<TypeItem, std::pair<uint32_t, uint32_t>> listItemsCatalog() const;
};
