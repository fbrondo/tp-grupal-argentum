#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>
#include "server/includes/core/item.h"
#include "server/includes/npc/citynpc.h"

class TraderNPC: public CityNPC {
protected:
    std::vector<TypeItem> store_items;
    //std::map<TypeItem, std::unique_ptr<Item>> store;

public:
    //TraderNPC(TypeNPC type, std::string&& name, Pose&& pos, std::map<TypeItem, std::unique_ptr<Item>>&& store);
    TraderNPC(TypeNPC type, const std::string& name, Pose&& pos, const std::vector<TypeItem>& items);

    virtual ~TraderNPC() = default;

    const std::map<TypeItem, std::unique_ptr<Item>>& getStore() const;
    bool hasItem(TypeItem type) const;
    InteractionResult interact() override;
};
