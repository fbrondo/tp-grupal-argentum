#include "server/includes/player.h"

#include <cstring>
#include <vector>

#include "common/includes/core/Statistics.h"
#include "server/includes/exceptions/invalid_buy_exception.h"
#include "server/includes/exceptions/invalid_sell_exception.h"
#include "server/includes/game_formulas.h"
#include "server/includes/world.h"
#include "server/print.h"

Player::Player(const Pose& pos, Inventory&& inv_, Character&& ch_, const PlayerData& data):
        CombatEntity(pos, data), ch(std::move(ch_)), inv(std::move(inv_)) {
    this->statics = this->ch.getStatistics();
    this->level = data.level == 0 ? 1 : data.level;
    this->max_hp = this->hpMax();
    this->hp = data.hp == 0 ? this->max_hp : std::min(data.hp, this->max_hp);
    this->mana = data.mana == 0 ? this->manaMax() : std::min(data.mana, this->manaMax());
    this->exp = data.exp;
    this->user.username = data.username;
    this->user.password = data.password;
    Print::imprimirCajaContenedora(data);
    /*Falta init inventario**/
}

Player::Player(const User& user_, const Pose& pose_, Character&& ch_,
               const PlayerStateInit& state_init):
        CombatEntity(pose_, 0, state_init.level), user(user_), ch(std::move(ch_)) {
    this->inv = Inventory(state_init.golden_init, state_init.max_slots, state_init.capacity_slot);
    this->statics = ch.getStatistics();
    this->max_hp = this->hpMax();
    this->hp = this->hpMax();
    this->mana = this->manaMax();
    this->exp = 0;
}

bool Player::hasEnoughMana(uint16_t mana_cost) const { return this->mana >= mana_cost; }

uint16_t Player::hpMax() {
    const uint16_t& hp_f_race = this->ch.getHpFactorRace();
    const uint16_t& hp_f_clase = this->ch.getHpFactorClase();
    return GameFormulas::calculationMaximunHp(this->statics.constitution, hp_f_race, hp_f_clase,
                                              this->level);
}

uint16_t Player::manaMax() {
    const uint16_t& mana_f_race = this->ch.getManaFactorRace();
    const uint16_t& mana_f_clase = this->ch.getManaFactorClase();
    return GameFormulas::calculationMaximunMana(this->statics.intelligence, mana_f_race,
                                                mana_f_clase, this->level);
}

bool Player::isNewbie() const { return this->level <= 12; }

bool Player::isValidOpponent(Player* other) const {
    if (!other) {
        return true; /*Mi oponente es un creature*/
    }
    if (other->isNewbie() || this->isNewbie()) {
        return false;
    }
    const uint16_t max_lvl = std::max(this->level, other->level);
    const uint16_t min_lvl = std::min(this->level, other->level);
    if (max_lvl - min_lvl > 10) {
        return false;
    }
    return true;
}

bool Player::hasEnoughGold(uint32_t price) const { return this->inv.getGolden() >= price; }

bool Player::isInventoryFull() const { return this->inv.isInventoryFull(); }

bool Player::addItemToInventory(const ItemInstance& instance) {
    if (this->inv.isInventoryFull()) {
        return false;
    }
    const auto item = instance.item;
    this->inv.addItemToInventory(item);
    return true;
}

void Player::addItemToInventory(const GoldBagInstance& instance) {
    this->inv.incrementGolden(instance.amount);
}

bool Player::addItemToInventory(const TreasureInstance& instance) {
    this->inv.incrementGolden(instance.amount_golden);
    if (this->inv.isInventoryFull()) {
        return false;
    }
    for (const auto& item_inst: instance.items) {
        if (this->inv.isInventoryFull()) {
            break;
        }
        const auto item = dynamic_cast<const Item*>(item_inst.item);
        this->inv.addItemToInventory(item);
    }
    return true;
}

const Item* Player::removeItemFromInventory(TypeItem type_item) {
    if (this->inv.isInventoryEmpty()) {
        return nullptr;
    }
    return this->inv.removeItemFromInventory(type_item);
}

void Player::buyItem(const Item* item) {
    const auto price = item->purchase_price;
    if (this->inv.getGolden() < price) {
        throw InvalidBuyException("No tenés suficiente oro para comprar ese objeto.");
    }
    if (this->inv.isInventoryFull()) {
        throw InvalidBuyException("Tu inventario está lleno.");
    }
    this->inv.decrementGolden(price);
    this->inv.addItemToInventory(item);
}

void Player::sellItem(TypeItem type_item, uint32_t sell_price) {
    if (this->inv.isInventoryEmpty()) {
        throw InvalidSellException("Tu inventario esta vacio.");
    }
    if (!this->inv.itemInInventory(type_item)) {
        throw InvalidSellException("No tienes este item en tu inventario.");
    }
    this->inv.incrementGolden(sell_price);
    this->inv.removeItemFromInventory(type_item);
}

bool Player::dropItem(size_t index_slot, World& world) {
    if (this->inv.slotEmpty(index_slot)) {
        return false;
    }
    const auto item = this->inv.getItemSlot(index_slot);
    ItemInstance instance(item);
    instance.position = world.findNearbyFreePosition(this->pose.position);
    world.addItemWorld(instance);
    this->inv.removeItemFromInventory(index_slot);
    return true;
}

bool Player::useItem(const size_t& slot_id) {
    if (this->inv.isInventoryEmpty()) {
        return false;
    }
    const auto item = this->inv.getItemSlot(slot_id);
    if (!item) {
        return false;
    }
    if (item->use(*this)) {
        this->inv.removeItemFromInventory(slot_id);
        return true;
    }
    return false;
}

bool Player::canAttack() const { return this->attack_cooldown_current == 0; }

bool Player::equipItem(size_t slot_id) {
    if (this->inv.slotEmpty(slot_id)) {
        return false;
    }
    return this->inv.setItemInTheEquipment(this->equipment, slot_id);
}

bool Player::unequipItem(size_t slot_id) {
    return this->inv.removeItemFromEquipment(this->equipment, slot_id);
}

PlayerData Player::getPlayerData() const {
    PlayerData data{};
    std::memset(data.username, 0, MAX_DATA);
    user.username.copy(data.username, MAX_DATA - 1);

    std::memset(data.password, 0, MAX_DATA);
    user.password.copy(data.password, MAX_DATA - 1);

    data.position = this->pose.position;
    data.direction = this->pose.direct;

    /*PERSONAJE*/
    data.charact_traits.race = static_cast<uint8_t>(this->ch.getTypeRace());
    data.charact_traits.clase = static_cast<uint8_t>(this->ch.getTypeClase());
    data.charact_traits.head = this->ch.getTypeHead();
    data.charact_traits.body = this->ch.getTypeBody();
    /*Atributos actuales*/
    data.exp = this->exp;
    data.level = this->level;
    data.hp = this->hp;
    data.mana = this->mana;
    /*INVENTARIO*/
    data.golden = this->inv.getGolden();
    data.inventory = this->inv.getSlotsData();
    /*EQUIPO*/
    for (const auto& slot: this->equipment.getEquipmentSlots()) {
        data.inventory.emplace_back(slot.type_item, data.inventory.size(), slot.quantity);
    }
    return data;
}

uint16_t Player::calculateDamage(bool& is_critical, Weapon& weapon) const {
    uint16_t min_dmg = weapon.minimal_damage;
    uint16_t max_dmg = weapon.maximun_damage;
    uint16_t strength = this->statics.strength;
    uint16_t damage = GameFormulas::calculationDamage(strength, min_dmg, max_dmg, is_critical);
    return damage;
}

uint16_t Player::calculateDefense(std::vector<Defense*> info_defense) {
    return GameFormulas::calculationDefense(info_defense);
}

PlayerSnapshotData Player::getPlayerSnapshotData(const Id& player_id) {
    PlayerSnapshotData data;
    const uint32_t oro_max = GameFormulas::calculationGoldenMax(this->level);
    const uint32_t golden = this->inv.getGolden();
    std::memset(data.name, 0, MAX_NAME_SIZE);
    user.username.copy(data.name, MAX_NAME_SIZE - 1);
    data.id = player_id;
    data.position.x = this->pose.position.x;
    data.position.y = this->pose.position.y;
    data.direction = this->pose.direct;
    data.stats.current_hp = this->hp;
    data.stats.max_hp = this->hpMax();
    data.stats.current_mana = this->mana;
    data.stats.max_mana = this->manaMax();
    data.stats.level = this->level;
    data.stats.xp = this->exp;
    data.stats.safe_gold = std::min(golden, oro_max);
    data.stats.excess_gold = (golden > oro_max) ? (golden - oro_max) : 0;
    data.ch_traits.body = this->ch.getTypeBody();
    data.ch_traits.head = this->ch.getTypeHead();
    data.ch_traits.race = this->ch.getTypeRace();
    data.ch_traits.clase = this->ch.getTypeClase();
    data.weapon_id = static_cast<uint8_t>(this->getHandItem());
    data.shield_id = static_cast<uint8_t>(this->equipment.getShieldItem());
    data.helmet_id = static_cast<uint8_t>(this->equipment.getHelmetItem());
    data.armor_id = static_cast<uint8_t>(this->equipment.getArmorItem());
    data.flags = 0;
    if (!this->isAlive())
        data.flags |= PLAYER_FLAG_GHOST;
    if (this->isMeditating())
        data.flags |= PLAYER_FLAG_MEDITATING;
    data.resurrection_time_left_ms = 0;
    return data;
}

TypeItem Player::getHandItem() { return this->equipment.getHandItem(); }

std::vector<TypeItem> Player::getEquipment() { return this->equipment.getEquipmentDefensive(); }

std::vector<MsgSlot> Player::getSlotsInventory() const { return this->inv.getInventory(); }

std::vector<MsgSlot> Player::getSlotsEquipment() const {
    return this->equipment.getEquipmentSlots();
}

void Player::teleportTo(const Position& pos) { this->pose.position = pos; }

std::string Player::getName() const { return this->user.username; }

const Item* Player::getItemInventory(const size_t& slot_id) {
    return this->inv.getItemSlot(slot_id);
}

const Item* Player::removeItemInventory(TypeItem type_item) {
    return this->inv.removeItemFromInventory(type_item);
}

uint32_t Player::decreaseGold(const uint32_t& amount) {
    if (amount <= this->inv.getGolden()) {
        this->inv.decrementGolden(amount);
        return amount;
    }
    auto gold_current = this->inv.getGolden();
    this->inv.decrementGolden(gold_current);
    return gold_current;
}

void Player::increaseGold(uint32_t amount) { this->inv.incrementGolden(amount); }

bool Player::isMeditating() const { return this->is_meditating; }

bool Player::isResurrecting() const { return this->is_resurrecting; }

void Player::toggleMeditation() {
    if (this->ch.getTypeClase() == WARRIOR)
        return;
    this->is_meditating = !this->is_meditating;
}

void Player::toggleInfiniteHp() { this->infinite_hp = !this->infinite_hp; }

void Player::toggleInfiniteMana() { this->infinite_mana = !this->infinite_mana; }

bool Player::hasInfiniteHp() const { return this->infinite_hp; }

bool Player::hasInfiniteMana() const { return this->infinite_mana; }

bool Player::breakMeditation() {
    if (!this->is_meditating)
        return false;
    this->is_meditating = false;
    return true;
}

void Player::updateHp(float delta) {
    if (!this->isAlive()) {
        return;
    }
    if (this->hp >= this->hpMax()) {
        return;
    }
    this->hp += GameFormulas::calculationRecoveredHp(this->ch.getHpFactorRace(), delta);
    this->hp = std::min(this->hp, this->hpMax());
}

void Player::updateMana(float delta) {
    if (this->mana >= this->manaMax()) {
        return;
    }
    this->mana += GameFormulas::calculationOfManaTime(this->ch.getHpFactorRace(), delta);
    this->mana = std::min(this->mana, this->manaMax());
}

void Player::meditating(float delta) {
    if (this->mana >= this->manaMax()) {
        return;
    }
    uint16_t fact_clase = ch.getManaFactorClase();
    this->mana += GameFormulas::calculationOfManaMeditation(fact_clase, this->statics.intelligence,
                                                            delta);
    this->mana = std::min(this->mana, this->manaMax());
}

void Player::restoreAllHp() { this->hp = this->hpMax(); }

void Player::receiveDamage(uint16_t damage, World& world) {
    if (this->infinite_hp) {
        return;
    }
    CombatEntity::receiveDamage(damage, world);
}

void Player::restoreAllMana() { this->mana = this->manaMax(); }

void Player::startResurrection() {
    this->is_resurrecting = true;
    this->breakMeditation();
}

void Player::finishResurrection() { this->is_resurrecting = false; }

void Player::earnExperiencePoints(CombatEntity* victim, uint16_t damage) {
    this->exp += GameFormulas::calculationPointsExpAttack(damage, victim->getLevel(), this->level);
    uint32_t limit = GameFormulas::limitMoveUpToNextLevel(this->level);
    if (this->exp >= limit) {
        this->level += 1;
    }
}

void Player::earnKillExp(CombatEntity* victim) {
    this->exp += GameFormulas::calculationPointsExpKill(victim->getMaxHp(), victim->getLevel(),
                                                        this->level);
    const uint32_t limit = GameFormulas::limitMoveUpToNextLevel(this->level);
    if (this->exp >= limit) {
        this->level += 1;
    }
}

void Player::consumeMana(uint16_t amount) {
    if (this->infinite_mana) {
        return;
    }
    this->mana = (this->mana >= amount) ? (this->mana - amount) : 0;
}


void Player::onDeath(World& world) {
    this->inv.dropInventory(world, this->pose.position);
    this->equipment.dropEquipment(world, this->pose.position);
    const uint16_t oro_max = GameFormulas::calculationGoldenMax(this->level);
    const uint16_t golden = this->inv.getGolden();
    if (golden > oro_max) {
        const uint16_t exceso = golden - oro_max;
        GoldBagInstance gold_pouche;
        gold_pouche.amount = exceso;
        gold_pouche.position = world.findNearbyFreePosition(this->pose.position);
        world.addItemWorld(gold_pouche);
        this->inv.decrementGolden(exceso);
    }
}
