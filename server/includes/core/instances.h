#ifndef INSTANCES_H
#define INSTANCES_H

#include "common/includes/direction.h"
#include "common/includes/types.h"

#include "item.h"
#include "map.h"

struct ItemInstace {
    Id id;
    TypeItem type;
    ItemClassification classification; /* DEFENSIVE, ATTACK, HEALING*/
    BodyPart body_part_use;
    Position pos; /*Nota cuando este equipado seguramente tenga la misma posicion que el jugador*/

    ItemInstace(const Id id_, TypeItem type, ItemClassification classif, BodyPart body_part):
            id(id_), type(type), classification(classif), body_part_use(body_part) {}
};

struct NpcInstance {
    // const Id id_instance; //unico, se genera en runtime
    TypeNPC type_npc;
    Position position;
    Direction
            direct; /*direccion de la mirada - Cuando interactue con un jugador, el NPC lo mirara*/
};

struct PlayerInstance {
    Position position;
    Direction direct; /*direccion de la mirada*/
    PlayerInstance() = default;
    PlayerInstance(Position position_, Direction direct_): position(position_), direct(direct_) {}
};

#endif
