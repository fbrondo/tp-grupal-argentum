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

/*Banquero - Interaccion:
    - comprar
    - vender
    - listar
*/
class Banker: public CityNPC {
public:
    Banker(TypeNPC type, std::string&& name, Pose&& pos);
    ~Banker() = default;
    InteractionResult interact() override;
};

#endif
