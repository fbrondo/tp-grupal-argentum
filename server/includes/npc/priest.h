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

/*SACERDOTE - Interaccion:
    - resucitar jugador
    - curar jugador (puede curar tanto mana como vida)
    - puede vender baculos, varas, pociones.
*/
class Priest: public TraderNPC {
private:
    /*Baculos, varas, pociones*/
    // std::map<TypeItem, std::unique_ptr<Item>> store;

public:
    // Priest(TypeNPC type, std::string&& name, Pose&& pos,
    //        std::map<TypeItem, std::unique_ptr<Item>>&& store_);
    Priest(TypeNPC type, const std::string& name, std::map<TypeItem, Item*>&& items_);
    ~Priest() override = default;
};

#endif
