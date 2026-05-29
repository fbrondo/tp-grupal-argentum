#ifndef INSTANCES_H
#define INSTANCES_H

#include "common/includes/types.h"
#include "item.h"
#include "map.h"
/*Esto solo cuando
    - Un jugador compre u obtenga un item para su inventario
    - En caso de que un item sea dropeado para identificarlo
*/
/*NOTA: esto puede ir en commmon, el cliente tambien puede llegar a usarlo*/
struct ItemInstace {
    Id id_instance;
    TypeItem type;
    ItemClassification classification; /* DEFENSIVE, ATTACK, HEALING*/
    BodyPart body_part_use;
    Position pos; /*Nota cuando este equipado seguramente tenga la misma posicion que el jugador*/
    
    ItemInstace(Id id, TypeItem type, ItemClassification classif, BodyPart body_part):
            id_instance(id),
            type(type),
            classification(classif),
            body_part_use(body_part) {}
};

struct NpcInstance {
    //Id id_instance; //unico, se genera en runtime
    TypeNPC type_npc;
    const Position& pos;
};

#endif
