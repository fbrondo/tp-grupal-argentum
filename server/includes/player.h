#ifndef PLAYER_H
#define PLAYER_H

#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "common/includes/core/snapshot.h"
#include "common/includes/core/user.h"
#include "common/includes/protocol.h"
#include "common/includes/types.h"
#include "entity/combat_entity.h"
#include "server/includes/character.h"
#include "server/includes/core/config.h"
#include "server/includes/core/data.h"
#include "server/includes/core/instances.h"
#include "server/includes/core/item.h"
#include "server/includes/core/map.h"
#include "server/includes/equipment.h"
#include "server/includes/inventory.h"

class Player: public CombatEntity {
private:
    bool is_meditating = false;
    bool is_resurrecting = false;
    bool infinite_hp = false;
    bool infinite_mana = false;

    uint16_t mana;
    uint32_t exp;

    User user;
    Equipment equipment;
    Character ch;
    Inventory inv;
    Statistics statics;

    uint16_t hpMax();
    uint16_t manaMax();

    float resurrection_timer = 0.0f;  // Tiempo restante en segundos
    Position healer_target_position;

public:
    Player(const Player& other) = delete;
    Player& operator=(const Player& other) = delete;
    Player(Player&&) = default;
    Player& operator=(Player&&) = default;

    explicit Player(const Pose& pos_, Inventory&& inv_, Character&& ch_, const PlayerData& data);
    explicit Player(const User& user, const Pose& pose, Character&& ch,
                    const PlayerStateInit& state_init);

    TypeItem getHandItem();
    std::vector<TypeItem> getEquipment();
    std::vector<MsgSlot> getSlotsInventory() const;
    std::vector<MsgSlot> getSlotsEquipment() const;

    std::string getName() const override;
    const Item* getItemInventory(const size_t& slot_id);
    const Item* removeItemInventory(TypeItem type_item);

    uint32_t decreaseGold(const uint32_t& amount);
    void increaseGold(uint32_t amount);

    bool isNewbie() const;
    bool isValidOpponent(Player* other) const;
    bool isMeditating() const;
    bool isResurrecting() const;
    bool hasEnoughMana(uint16_t mana_cost) const;
    bool hasEnoughGold(uint32_t price) const;
    bool isInventoryFull() const;

    bool addItemToInventory(const ItemInstance& instance);
    void addItemToInventory(const GoldBagInstance& instance);
    bool addItemToInventory(const TreasureInstance& instance);
    const Item* removeItemFromInventory(TypeItem type_item);

    void teleportTo(const Position& pos);
    void toggleMeditation();
    void toggleInfiniteHp();
    void toggleInfiniteMana();
    bool hasInfiniteHp() const;
    bool hasInfiniteMana() const;
    bool breakMeditation();
    void updateHp(float delta);
    void updateMana(float delta);
    void meditating(float delta);
    void restoreAllMana();
    void restoreAllHp();

    void receiveDamage(uint16_t damage, World& world) override;

    void startResurrection();
    void finishResurrection();
    void earnExperiencePoints(CombatEntity* victim, uint16_t damage);
    void earnKillExp(CombatEntity* victim);
    void consumeMana(uint16_t amount);

    void buyItem(const Item* item);
    void sellItem(TypeItem type_item, uint32_t sell_price);
    bool dropItem(size_t index, World& world);
    bool equipItem(size_t slot_id);
    bool unequipItem(size_t slot_id);
    bool useItem(const size_t& slot_id);
    bool canAttack() const;

    uint16_t calculateDamage(bool& is_critical, Weapon& weapon) const;
    static uint16_t calculateDefense(std::vector<Defense*> info_defense);

    PlayerSnapshotData getPlayerSnapshotData(const Id& id);
    PlayerData getPlayerData() const;

    void onDeath(World& world) override;  // El jugador se convierte en fantasma

    ~Player() override = default;
};


#endif
