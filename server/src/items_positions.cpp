#include "server/includes/items_positions.h"

void ItemsPositions::add(const ItemInstance& item) {
    this->items_on_floor.emplace(item.id, item);
    this->items_tiles.emplace(item.position, true);
}

void ItemsPositions::add(const GoldBagInstance& gold_bag) {
    this->gold_bags_on_floor.emplace(gold_bag.id, gold_bag);
    this->items_tiles.emplace(gold_bag.position, true);
}

void ItemsPositions::add(const TreasureInstance& treasure) {
    this->treasures_on_floor.emplace(treasure.id, treasure);
    this->items_tiles.emplace(treasure.position, true);
}

void ItemsPositions::remove(const ItemInstance& item) { items_on_floor.erase(item.id); }
void ItemsPositions::remove(const GoldBagInstance& gold) {
    this->gold_bags_on_floor.erase(gold.id);
}
void ItemsPositions::remove(const TreasureInstance& t) { treasures_on_floor.erase(t.id); }

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

// Si el jugador solo manda "Quiero interactuar con lo que hay en la posición X, Y"
// Podés hacer un método de búsqueda por posición que barra los mapas:
// void interactAt(const Position& pos) {
//     // Buscamos en ítems
//     for (auto it = items_on_floor.begin(); it != items_on_floor.end(); ++it) {
//         if (it->second.pos == pos) {
//             // Lógica de agarrar ítem...
//             items_on_floor.erase(it);
//             return;
//         }
//     }
//     // Repetir lógica para bolsas de oro y tesoros...
// }

// const std::unordered_map<Id, ItemInstance>& getItems() const { return items_on_floor; }
// const std::unordered_map<Id, GoldBagInstance>& getGoldBags() const { return gold_bags_on_floor; }
// const std::unordered_map<Id, TreasureInstance>& getTreasures() const { return treasures_on_floor;
// }
