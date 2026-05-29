#ifndef MERCHANT_H
#define MERCHANT_H

#include <map>
#include <memory>
#include <string>
#include <utility>

#include "../core/item.h"
#include "../core/map.h"
#include "common/includes/types.h"

//#include "../commands/command.h"
#include "../player.h"

#include "citynpc.h"

class Comand;
class World;

/*COMERCIANTE - Interaccion:
    - comprar
    - vender
*/
class Merchant: public CityNPC {

private:
    /*Toda clase de Items menos items magicos*/
    std::map<TypeItem, std::unique_ptr<Item>> store;

public:
    Merchant(TypeNPC type, std::string&& name, Position&& pos,
             std::map<TypeItem, std::unique_ptr<Item>> store);
    ~Merchant() = default;
    //void interact(const Id& id_player, World& word, Comand& cmd) override;
};

#endif
