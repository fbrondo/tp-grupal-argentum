#ifndef INSTANCES_H
#define INSTANCES_H

#include "common/includes/direction.h"
#include "common/includes/types.h"
#include "server/includes/core/item.h"
#include "server/includes/core/map.h"

struct ItemInstance {
    Id id;
    TypeItem type;
    ItemClassification classification; /* DEFENSIVE, ATTACK, HEALING*/
    BodyPart body_part_use;
    Position pos; /*Nota cuando este equipado seguramente tenga la misma posicion que el jugador*/

    ItemInstance(const Id id_, TypeItem type, ItemClassification classif, BodyPart body_part,
                 Position pos):
            id(id_), type(type), classification(classif), body_part_use(body_part), pos(pos) {}
};

struct NpcInstance {
    // const Id id_instance; //unico, se genera en runtime
    TypeNPC type;
    Pose pose;
    NpcInstance() = default;
    NpcInstance(TypeNPC type_npc, const Pose& pose): type(type_npc), pose(pose) {}

};

// struct PlayerInstance {
//     Position position;
//     Direction direct; /*direccion de la mirada*/
//     PlayerInstance() = default;
//     PlayerInstance(Position position_, Direction direct_): position(position_), direct(direct_)
//     {}
// };

#endif
