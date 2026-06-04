#ifndef BANKER_H
#define BANKER_H

#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "common/includes/types.h"
#include "server/includes//player.h"
#include "server/includes/core/item.h"
#include "server/includes/core/map.h"
#include "server/includes/npc/citynpc.h"

class Comand;
class World;
struct BanckAccount {
    uint16_t golden;
    std::vector<std::unique_ptr<ItemInstance>> safe_box;
};

/*Banquero - Interaccion:
    - comprar
    - vender
    - listar
*/
class Banker: public CityNPC {

private:
    /*Toda clase de Items menos items magicos*/
    std::map<TypeItem, std::unique_ptr<Item>> store;

public:
    Banker(TypeNPC type, std::string&& name, Pose&& pos);
    ~Banker() = default;
    // void interact(const Id& id_player, World& word, Comand& cmd) override;
};

#endif
