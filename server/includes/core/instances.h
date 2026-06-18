#ifndef INSTANCES_H
#define INSTANCES_H

#include <vector>

#include "common/includes/core/position.h"
#include "common/includes/types.h"
#include "server/includes/core/item.h"
#include "server/includes/core/map.h"

struct ItemInstance {
    Id id{0};
    Position position;
    const Item* item{nullptr};
    // bool is_equipped{false};

    ItemInstance() = default;
    explicit ItemInstance(const Item* item_): item(item_) {}
    ItemInstance(const Id id_, const Item* item_): id(id_), item(item_) {}

    ItemInstance(ItemInstance&&) = default;
    ItemInstance& operator=(ItemInstance&&) = default;

    ItemInstance(const ItemInstance&) = default;
    ItemInstance& operator=(const ItemInstance&) = default;
};

struct GoldBagInstance {
    Id id{0};
    uint32_t amount{0};
    Position position;

    GoldBagInstance() = default;
    GoldBagInstance(Position pos_, uint32_t amount_): amount(amount_), position(pos_) {}

    GoldBagInstance(GoldBagInstance&&) = default;
    GoldBagInstance& operator=(GoldBagInstance&&) = default;

    GoldBagInstance(const GoldBagInstance&) = default;
    GoldBagInstance& operator=(const GoldBagInstance&) = default;
};

struct TreasureInstance {
    Id id{0};
    Id zone_id{0};
    Position position;
    uint32_t amount_golden{0};
    std::vector<ItemInstance> items;  // El tesoro contiene múltiples ítems adentro

    TreasureInstance() = default;
    TreasureInstance(TreasureInstance&&) = default;
    TreasureInstance& operator=(TreasureInstance&&) = default;
    TreasureInstance(const TreasureInstance&) = default;
    TreasureInstance& operator=(const TreasureInstance&) = default;
};

struct NpcInstance {
    Id id{0};
    Id zone_id;
    TypeNPC type;
    Pose pose;

    NpcInstance() = default;
    NpcInstance(const Id& id_, const Id& zone_id_, TypeNPC type_, const Pose& pose):
            id(id_), zone_id(zone_id_), type(type_), pose(pose) {}
    NpcInstance(NpcInstance&&) = default;
    NpcInstance& operator=(NpcInstance&&) = default;
    NpcInstance(const NpcInstance&) = default;
    NpcInstance& operator=(const NpcInstance&) = default;
};
#endif
