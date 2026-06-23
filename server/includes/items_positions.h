#ifndef ITEMS_POSITIONS_H
#define ITEMS_POSITIONS_H

#include <unordered_map>
#include <variant>
#include <vector>

#include "common/includes/types.h"
#include "server/includes/core/data.h"
#include "server/includes/core/instances.h"
#include "server/includes/player.h"

using GroundContent = std::variant<std::monostate, ItemInstance, GoldBagInstance, TreasureInstance>;
class ItemsPositions {
private:
    std::unordered_map<Id, ItemInstance> items_on_floor;
    std::unordered_map<Id, GoldBagInstance> gold_bags_on_floor;
    std::unordered_map<Id, TreasureInstance> treasures_on_floor;
    std::unordered_map<Position, bool, PositionHash> items_tiles;

public:
    ItemsPositions() = default;

    void add(const ItemInstance& item);
    void add(const GoldBagInstance& gold_bag);
    void add(const TreasureInstance& treasure);
    bool removeItemTakeToPlayer(Player& player);

    bool isOcupied(const Position& position) const;

    std::vector<ItemGroundSnapshotData> getItemsOnFloor();
    std::vector<ItemInstanceData> getDataItems();
    std::vector<TreasureStateData> getDataTreasures();
    std::vector<GoldBagsData> getDataGoldBags();
};


#endif
