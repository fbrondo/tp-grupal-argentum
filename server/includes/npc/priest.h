#ifndef PRIEST_H
#define PRIEST_H

#include <map>
#include <memory>
#include <string>

#include "common/includes/types.h"
#include "server/includes/core/item.h"
#include "server/includes/core/map.h"
#include "server/includes/npc/trader.h"

class World;

class Priest: public TraderNPC {
private:
public:
    Priest(const std::string& name, const NpcInstance& instance,
           std::map<TypeItem, Item*>&& items_);
    void heal(Player& player) const;
    void resurrect(Player& player, World& world, Id player_id) const;
    ~Priest() override = default;
};

#endif
