#include "server/includes/npc/criature.h"

Creature::Creature(TypeNPC type, Pose&& pos, uint16_t r_attack, uint16_t hp_max, uint16_t level):
        CombatEntity(std::move(pos), hp_max),
       type_creature(type),
        // drop_gold(0),
        range_attack(r_attack),
        level(level) {}

// void Creature::updatePosition(Position&& new_pos) { this->pos = std::move(new_pos); }

void Creature::onDeath() {
    uint16_t drop_gold = GameFormulas::calculationGoldenNpcKill(this->max_hp);
    // Elegimos al azar un item de su pool de objetos
    TypeItem item_to_drop = NONE;
    if (!this->drop_items_pool.empty()) {
        std::random_device rd;
        std::mt19937 rng(rd());
        std::uniform_int_distribution<size_t> item_dist(0, this->drop_items_pool.size() - 1);
        item_to_drop = this->drop_items_pool[item_dist(rng)];
    }
    //Position death_pos = this->getPosition();

    if (item_to_drop != NONE) {
       // this->world.spawnItemOnFloor(death_pos, item_to_drop);
    }

    if (drop_gold > 0) {
        //this->world.spawnGoldOnFloor(death_pos, drop_gold);
    }
    // this->markForRemoval();
}
