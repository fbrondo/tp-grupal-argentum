#include "../../includes/npc/criature.h"

Creature::Creature(Id id, TypeNPC type, Position&& pos, uint16_t r_attack, uint16_t hp_max,
                   uint16_t level, GameFormulas& formulas, World& world) :
        CombatEntity(id, std::move(pos), hp_max, formulas),
        world(world),
        type_creature(type),
        drop_gold(0),
        range_attack(r_attack),
        level(level) {}

void Creature::updatePosition(Position&& new_pos) { this->pos = std::move(new_pos); }

void Creature::onDeath() {

    this->drop_gold = this->form.calculationGoldenNpcKill(this->max_hp);

    // Elegimos al azar un item de su pool de objetos
    TypeItem item_to_drop = TypeItem::NONE;
    if (!this->drop_items_pool.empty()) {
        std::uniform_int_distribution<size_t> item_dist(0, this->drop_items_pool.size() - 1);
        item_to_drop = this->drop_items_pool[item_dist(this->form.getRng())];
    }

    Position death_pos = this->getPosition();

    if (item_to_drop != TypeItem::NONE) {
        this->world.spawnItemOnFloor(death_pos, item_to_drop);
    }
    
    if (this->drop_gold > 0) {
        this->world.spawnGoldOnFloor(death_pos, this->drop_gold);
    }
    // this->markForRemoval();
}
