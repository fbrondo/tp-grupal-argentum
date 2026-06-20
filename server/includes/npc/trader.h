#pragma once

#include <map>
#include <memory>
#include <string>

#include "server/includes/core/item.h"
#include "server/includes/npc/citizen_npc.h"
#include "server/includes/player.h"

class TraderNPC: public CitizenNPC {
protected:
    std::map<TypeItem, Item*> store;

public:
    TraderNPC(TypeNPC type, const std::string& name, const Pose& pose_,
              std::map<TypeItem, Item*>&& items);
    ~TraderNPC() override = default;
    virtual bool executeBuyItem(Player& player, TypeItem type_item_buy);
    virtual std::map<TypeItem, uint32_t> listItemsCatalog() const;
};
