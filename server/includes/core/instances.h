#ifndef INSTANCES_H
#define INSTANCES_H

#include "common/includes/types.h"
#include "server/includes/core/item.h"
#include "server/includes/core/map.h"

struct ItemInstance {
    Id id{0};
    TypeItem type{NONE};
    ItemClassification classification{NO_CLASSIF}; /* DEFENSIVE, ATTACK, HEALING*/
    BodyPart body_part_use{NO_BODY};
    Position pos; /*Nota cuando este equipado seguramente tenga la misma posicion que el jugador*/
    ItemInstance() = default;

    ItemInstance(const Id id_, TypeItem type, ItemClassification classif, BodyPart body_part):
            id(id_), type(type), classification(classif), body_part_use(body_part) {}

    ItemInstance(const Id id_, TypeItem type, ItemClassification classif, BodyPart body_part,
                 Position pos):
            id(id_), type(type), classification(classif), body_part_use(body_part), pos(pos) {}

    ItemInstance(ItemInstance&&) = default;
    ItemInstance& operator=(ItemInstance&&) = default;

    ItemInstance(const ItemInstance&) = default;
    ItemInstance& operator=(const ItemInstance&) = default;
};

struct NpcInstance {
    TypeNPC type;
    Pose pose;
    NpcInstance() = default;
    NpcInstance(TypeNPC type_npc, const Pose& pose): type(type_npc), pose(pose) {}
};

struct GoldBagInstance {
    uint32_t amount{0};
    Position pos;

    GoldBagInstance() = default;
    GoldBagInstance(Position pos_, uint32_t amount_): amount(amount_), pos(pos_) {}

    GoldBagInstance(GoldBagInstance&&) = default;
    GoldBagInstance& operator=(GoldBagInstance&&) = default;

    GoldBagInstance(const GoldBagInstance&) = default;
    GoldBagInstance& operator=(const GoldBagInstance&) = default;
};
#endif
