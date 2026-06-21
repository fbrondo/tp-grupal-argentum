#include "server/includes/npc/creature.h"

#include <algorithm>
#include <cstring>
#include <stdexcept>

#include "common/includes/types.h"
#include "server/print.h"


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

Creature::Creature(const Id& id_, const std::string& name_, TypeNPC type_, const Pose& pos,
                   const NpcAttributes& attrib, std::vector<ItemInstance>&& items_):
        CombatEntity(pos, attrib.max_hp, attrib.difficulty_level),
        id(id_),
        name(name_),
        type_creature(type_),
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
            world.addItemWorld(gold);
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

CreatureData Creature::getCreatureData() const {
    CreatureData creauture_npc;
    creauture_npc.type = this->type_creature;
    creauture_npc.attributes.current_hp = this->hp;
    creauture_npc.attributes.max_hp = this->max_hp;
    creauture_npc.attributes.range_attack = this->range_attack;
    creauture_npc.position = this->pose.position;
    creauture_npc.direction = this->pose.direct;
    return creauture_npc;
}

NpcSnapshotData Creature::getNpcSnapshotData() const {
    NpcSnapshotData snapshot;
    std::memset(snapshot.name, 0, MAX_NAME_SIZE);
    this->name.copy(snapshot.name, MAX_NAME_SIZE - 1);
    snapshot.id = this->id;
    snapshot.type_id = this->type_creature;
    snapshot.direction = this->pose.direct;
    snapshot.current_hp = this->hp;
    snapshot.max_hp = this->max_hp;
    snapshot.position.x = this->pose.position.x;
    snapshot.position.y = this->pose.position.y;
    return snapshot;
}

std::string Creature::getName() const { return this->name; }

uint16_t Creature::getAggroRange() const { return this->range_attack; }

bool Creature::canAttack() const { return this->attack_cooldown_current == 0; }

bool Creature::canMove() const { return this->movement_cooldown_current == 0; }

void Creature::resetAttackCooldown(uint32_t cooldown_ms) {
    this->attack_cooldown_current = cooldown_ms;
}

void Creature::resetMovementCooldown(uint32_t cooldown_ms) {
    this->movement_cooldown_current = cooldown_ms;
}

void Creature::updateCooldowns(uint32_t delta_ms) {
    this->attack_cooldown_current = delta_ms >= this->attack_cooldown_current ?
                                            0 :
                                            this->attack_cooldown_current - delta_ms;
    this->movement_cooldown_current = delta_ms >= this->movement_cooldown_current ?
                                              0 :
                                              this->movement_cooldown_current - delta_ms;
}

uint16_t Creature::calculateDamage(bool& is_critical) const {
    return GameFormulas::calculationDamage(std::max<uint16_t>(1, this->level), 1, 2, is_critical);
}
