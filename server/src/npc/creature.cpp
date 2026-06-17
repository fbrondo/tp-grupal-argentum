#include "server/includes/npc/creature.h"

#include <algorithm>
#include <stdexcept>

#include "common/includes/types.h"

// #include "server/includes/entity.h"

ItemInstance Creature::search_item_drop(TypeItem type) {
    ItemInstance drop_item;
    auto it = std::find_if(
            this->items_to_drop.begin(), this->items_to_drop.end(),
            [&type](const ItemInstance& instance) { return instance.item->type == type; });
    if (it != this->items_to_drop.end()) {
        drop_item = *it;
    }
    return drop_item;
}

Creature::Creature(const Id& id_, TypeNPC type, const Pose& pos, const NpcAttributes& attrib,
                   std::vector<ItemInstance>&& items_):
        CombatEntity(pos, attrib.max_hp, attrib.difficulty_level),
        id(id_),
        type_creature(type),
        range_attack(attrib.range_attack),
        items_to_drop(std::move(items_)) {}

// void Creature::updatePosition(Position&& new_pos) { this->pos = std::move(new_pos); }

void Creature::onDeath(World& world) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::discrete_distribution<int> dist({80, 8, 1, 1});
    switch (dist(gen)) {
        case 0: /*nada*/
            break;
        case 1: { /*8% - oro*/
            const uint32_t drop_gold = GameFormulas::calculationGoldenNpcKill(this->max_hp);
            GoldBagInstance gold;
            gold.amount = drop_gold;
            gold.position = world.findNearbyFreePosition(this->pose.position);
            world.addGoldWorld(gold);
            break;
        }
        case 2: {  // 1% - pocion de vida o mana
            std::uniform_int_distribution<int> potion_dist(0, 1);
            TypeItem potion = (potion_dist(gen) == 0) ? LIFE_POTION : MANA_POTION;
            ItemInstance drop_potion = this->search_item_drop(potion);
            drop_potion.position = world.findNearbyFreePosition(this->pose.position);
            world.addItemWorld(drop_potion);
            break;
        }
        case 3: {  // 1% - cualquier otro objeto
            std::uniform_int_distribution<size_t> item_dist(0, this->items_to_drop.size() - 1);
            ItemInstance item_drop;
            do {
                item_drop = this->items_to_drop[item_dist(gen)];
            } while (item_drop.item->type == LIFE_POTION || item_drop.item->type == MANA_POTION);
            item_drop.position = world.findNearbyFreePosition(this->pose.position);
            world.addItemWorld(item_drop);
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
    creauture_npc.attributes.current_hp = this->hp;
    creauture_npc.attributes.max_hp = this->max_hp;
    creauture_npc.attributes.range_attack = this->range_attack;
    creauture_npc.position = this->pose.position;
    creauture_npc.direction = this->pose.direct;
    return creauture_npc;
}

NpcSnapshotData Creature::getNpcSnapshotData() {
    NpcSnapshotData npc_snapshot;
    npc_snapshot.id = this->id;
    npc_snapshot.type_id = this->type_creature;
    npc_snapshot.hp_actual = this->hp;
    npc_snapshot.pos_x = this->pose.position.x;
    npc_snapshot.pos_y = this->pose.position.y;
    npc_snapshot.is_alive = this->isAlive();
    return npc_snapshot;
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
