#ifndef CITYNPC_H
#define CITYNPC_H

#include <string>
#include <utility>

#include "common/includes/types.h"
#include "server/includes/core/map.h"
#include "server/includes/npc/npc.h"


/*Representa un npc de ciudad/pueblo
    - Banquero
    - Sacerdote
    - Comerciante
*/
class CityNPC: public NPC {
public:
    CityNPC(TypeNPC type, const std::string& name, Pose&& pos): NPC(type, name, std::move(pos)) {}
    virtual ~CityNPC() = default;
};

#endif
