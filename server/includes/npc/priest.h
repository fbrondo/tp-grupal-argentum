#ifndef PRIEST_H
#define PRIEST_H

#include <map>
#include <memory>
#include <string>

#include "common/includes/types.h"
#include "server/includes/core/item.h"
#include "server/includes/core/map.h"
#include "server/includes/npc/trader.h"
// #include "command.h"
#include "server/includes/player.h"


class Comand;
class World;

/*SACERDOTE - Interaccion:
    - resucitar jugador
    - curar jugador (puede curar tanto mana como vida)
    - puede vender baculos, varas, pociones.
*/
class Priest: public CityNPC {

private:
    /*Baculos, varas, pociones*/
    std::map<TypeItem, std::unique_ptr<Item>> store;

public:
    Priest(TypeNPC type, std::string&& name, Pose&& pos,
           std::map<TypeItem, std::unique_ptr<Item>>&& store_);
    ~Priest() = default;
};

#endif
