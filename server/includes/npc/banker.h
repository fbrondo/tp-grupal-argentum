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
    std::map<Id, BanckAccount> bank ;

public:
    Banker(TypeNPC type, const std::string& name, Pose&& pos);
    ~Banker() = default;
    InteractionResult interact() override;
};

#endif
