#ifndef MERCHANT_H
#define MERCHANT_H

#include <map>
#include <memory>
#include <string>
#include <utility>

#include "common/includes/types.h"
#include "server/includes/core/item.h"
#include "server/includes/core/map.h"
#include "server/includes/npc/trader.h"
#include "server/includes/player.h"

class Comand;

/*COMERCIANTE - Interaccion:
    - comprar
    - vender
*/
class Merchant: public TraderNPC {
public:
    // Merchant(TypeNPC type, const std::string& name, Pose&& pos,
    //          std::map<TypeItem, std::unique_ptr<Item>>&& store);
    Merchant(TypeNPC type, const std::string& name, Pose&& pos, std::vector<TypeItem> items_);
    ~Merchant() = default;
};

#endif
