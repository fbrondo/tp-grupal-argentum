#ifndef PRIEST_H
#define PRIEST_H

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "common/includes/types.h"
#include "server/includes/core/item.h"
#include "server/includes/core/map.h"
#include "server/includes/npc/trader.h"
#include "server/includes/player.h"

class Priest: public TraderNPC {
private:
public:
    Priest(TypeNPC type, const std::string& name, const Pose& pose_,
           std::map<TypeItem, Item*>&& items_);
    ~Priest() override = default;
};

#endif
