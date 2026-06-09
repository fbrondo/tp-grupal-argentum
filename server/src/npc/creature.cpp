#include "server/includes/npc/creature.h"

#include <algorithm>
#include <stdexcept>

#include "common/includes/types.h"

// #include "server/includes/entity.h"

ItemInstance Creature::search_item_drop(TypeItem type) {
    ItemInstance drop_item;
    auto it = std::find_if(this->items_to_drop.begin(), this->items_to_drop.end(),
                           [&type](const ItemInstance& item) { return item.type == type; });
    if (it != this->items_to_drop.end()) {
        drop_item = *it;
    }
    return drop_item;
}

Creature::Creature(const Id& id_, TypeNPC type, const Pose& pos, const NpcAttributes& attrib,
                   const std::vector<ItemInstance>& items_):
        CombatEntity(pos, attrib.hp_max, attrib.difficulty_level),
        id(id_),
        type_creature(type),
        range_attack(attrib.range_attack),
        items_to_drop(items_) {}

// void Creature::updatePosition(Position&& new_pos) { this->pos = std::move(new_pos); }

void Creature::onDeath(World& world) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::discrete_distribution<int> dist({80, 8, 1, 1});
    int result = dist(gen);
    switch (result) {
        case 0: /*nada*/
            break;
        case 1: { /*8% - oro*/
            uint32_t drop_gold = GameFormulas::calculationGoldenNpcKill(CombatEntity::max_hp);
            GoldBagInstance gold_pouche;
            gold_pouche.amount = drop_gold;
            gold_pouche.pos = world.findNearbyFreePosition(this->pose.position);
            world.spawnGoldOnFloor(gold_pouche);
            break;
        }
        case 2: {  // 1% - pocion de vida o mana
            std::uniform_int_distribution<int> potion_dist(0, 1);
            TypeItem potion = (potion_dist(gen) == 0) ? LIFE_POTION : MANA_POTION;
            ItemInstance drop_potion = this->search_item_drop(potion);
            drop_potion.pos = world.findNearbyFreePosition(this->pose.position);
            world.spawnItemOnFloor(drop_potion);
            break;
        }
        case 3: {  // 1% - cualquier otro objeto
            std::uniform_int_distribution<size_t> item_dist(0, this->items_to_drop.size() - 1);
            ItemInstance item_drop;
            do {
                item_drop = this->items_to_drop[item_dist(gen)];
            } while (item_drop.type == LIFE_POTION || item_drop.type == MANA_POTION);
            item_drop.pos = world.findNearbyFreePosition(this->pose.position);
            world.spawnItemOnFloor(item_drop);
            break;
        }
        default:
            throw std::runtime_error("Error en OnDead - Creature");
    }
    world.removeCreature(this->id);
}

CreatureData Creature::getCreatureData() {
    CreatureData creauture_npc;
    creauture_npc.type = this->type_creature;
    creauture_npc.hp = this->hp;
    creauture_npc.max_hp = this->max_hp;
    creauture_npc.x = this->pose.position.x;
    creauture_npc.y = this->pose.position.y;
    creauture_npc.direction = this->pose.direct;
    creauture_npc.range_attack = this->range_attack;
    return creauture_npc;
}


// // Elegimos al azar un item de su pool de objetos
// TypeItem item_to_drop = NONE;
// static std::random_device rd; std::mt19937 rng(rd());
// std::uniform_int_distribution<size_t> item_dist(0, this->drop_items_pool.size() - 1);
// item_to_drop = this->drop_items_pool[item_dist(rng)];
//
// Position death_pos = this->getPosition();
//
// if (item_to_drop != NONE) {
//     // this->world.spawnItemOnFloor(death_pos, item_to_drop);
// }
//
// if (drop_gold > 0) {
//     // this->world.spawnGoldOnFloor(death_pos, drop_gold);
// }
// // this->markForRemoval();
