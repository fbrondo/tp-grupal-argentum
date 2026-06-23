#include "server/includes/items_positions.h"

#include <iostream>
#include <ranges>

void ItemsPositions::add(const ItemInstance& item) {
    std::cerr << "[FLOOR] add ItemInstance id=" << item.id
              << " type=" << static_cast<int>(item.item ? item.item->type : 0) << " pos=("
              << item.position.x << "," << item.position.y << ")\n";
    this->items_on_floor.emplace(item.id, item);
    this->items_tiles.emplace(item.position, true);
}

void ItemsPositions::add(const GoldBagInstance& gold_bag) {
    std::cerr << "[FLOOR] add GoldBag id=" << gold_bag.id << " amount=" << gold_bag.amount
              << " pos=(" << gold_bag.position.x << "," << gold_bag.position.y << ")\n";
    this->gold_bags_on_floor.emplace(gold_bag.id, gold_bag);
    this->items_tiles.emplace(gold_bag.position, true);
}

void ItemsPositions::add(const TreasureInstance& treasure) {
    this->treasures_on_floor.emplace(treasure.id, treasure);
    this->items_tiles.emplace(treasure.position, true);
}

bool ItemsPositions::removeItemTakeToPlayer(Player& player) {
    bool item_take = false;
    const Position& position = player.getPosition();
    if (!this->isOcupied(position)) {
        return item_take;
    }
    for (auto it = this->items_on_floor.begin(); it != this->items_on_floor.end(); ++it) {
        if (it->second.position == position) {
            item_take = player.addItemToInventory(it->second);
            this->items_on_floor.erase(it);
            this->items_tiles.erase(position);
            break;
        }
    }

    for (auto it = this->gold_bags_on_floor.begin(); it != this->gold_bags_on_floor.end(); ++it) {
        if (it->second.position == position) {
            player.addItemToInventory(it->second);  // El jugador procesa el oro
            this->gold_bags_on_floor.erase(it);
            this->items_tiles.erase(position);
            item_take = true;
            break;
        }
    }

    for (auto it = this->treasures_on_floor.begin(); it != this->treasures_on_floor.end(); ++it) {
        if (it->second.position == position) {
            item_take = player.addItemToInventory(it->second);  // El jugador procesa el oro
            this->treasures_on_floor.erase(it);
            this->items_tiles.erase(position);
            break;
        }
    }
    return item_take;
}


bool ItemsPositions::isOcupied(const Position& position) const {
    return this->items_tiles.contains(position);
}

std::vector<ItemGroundSnapshotData> ItemsPositions::getItemsOnFloor() {
    std::vector<ItemGroundSnapshotData> items;
    for (auto it = this->items_on_floor.begin(); it != this->items_on_floor.end(); ++it) {
        ItemGroundSnapshotData item{};
        item.item_id = it->second.item->type;
        item.position = it->second.position;
        items.emplace_back(item);
    }
    for (auto it = this->treasures_on_floor.begin(); it != this->treasures_on_floor.end(); ++it) {
        ItemGroundSnapshotData item{};
        item.item_id = TREASURE;
        item.position = it->second.position;
        items.emplace_back(item);
    }
    for (auto it = this->gold_bags_on_floor.begin(); it != this->gold_bags_on_floor.end(); ++it) {
        ItemGroundSnapshotData item{};
        item.item_id = GOLD;
        item.position = it->second.position;
        items.emplace_back(item);
    }
    return items;
}

std::vector<ItemInstanceData> ItemsPositions::getDataItems() {
    std::vector<ItemInstanceData> items_data;
    for (const auto& instance: this->items_on_floor | std::views::values) {
        ItemInstanceData data{};
        data.type_item = static_cast<uint8_t>(instance.item->type);
        data.position = instance.position;
        items_data.push_back(data);
    }
    return items_data;
}

std::vector<TreasureStateData> ItemsPositions::getDataTreasures() {
    std::vector<TreasureStateData> treasures_data;
    for (const auto& instance: this->treasures_on_floor | std::views::values) {
        TreasureStateData data{};
        data.zone_id = instance.zone_id;
        data.position = instance.position;
        data.amount = instance.amount_golden;
        for (const auto& t: instance.items) {
            data.types_items.push_back(t.item->type);
        }
        treasures_data.push_back(data);
    }
    return treasures_data;
}

std::vector<GoldBagsData> ItemsPositions::getDataGoldBags() {
    std::vector<GoldBagsData> gold_bags_data;
    for (const auto& instance: this->gold_bags_on_floor | std::views::values) {
        GoldBagsData data{};
        data.position = instance.position;
        data.amount = instance.amount;
        gold_bags_data.emplace_back(data);
    }
    return gold_bags_data;
}
