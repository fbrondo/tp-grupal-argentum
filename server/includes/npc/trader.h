#pragma once

#include <map>
#include <memory>
#include <string>

#include "core/item.h"

#include "citynpc.h"

class TraderNPC: public CityNPC {
protected:
    std::map<TypeItem, std::unique_ptr<Item>> store;

public:
    TraderNPC(TypeNPC type, std::string&& name, Pose&& pos,
              std::map<TypeItem, std::unique_ptr<Item>>&& store);

    virtual ~TraderNPC() = default;

    const std::map<TypeItem, std::unique_ptr<Item>>& getStore() const;
    bool hasItem(TypeItem type) const;
    InteractionResult interact() override;
};
