#pragma once

#include <map>
#include <memory>
#include "citynpc.h"
#include "core/item.h"

class TraderNPC : public CityNPC {
protected:
    std::map<TypeItem, std::unique_ptr<Item>> store;

public:
    TraderNPC(TypeNPC type, std::string&& name, Pose&& pos,
              std::map<TypeItem, std::unique_ptr<Item>>&& store);

    virtual ~TraderNPC() = default;

    // Colocás acá los métodos compartidos de comercio
    const std::map<TypeItem, std::unique_ptr<Item>>& getStore() const;
    bool hasItem(TypeItem type) const;
};