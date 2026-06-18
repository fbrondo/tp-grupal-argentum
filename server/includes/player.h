#ifndef PLAYER_H
#define PLAYER_H

#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "../../common/includes/core/snapshot.h"
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

    uint16_t mana;
    uint16_t exp;

    User user;
    Equipment equipment;
    Character ch;
    Inventory inv;
    Statistics statics;

    uint16_t hpMax();
    uint16_t manaMax();

    float resurrection_timer = 0.0f;  // Tiempo restante en segundos
    Position healer_target_position;

    bool canBuy(uint16_t price_item) const;

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
    // Inventory& getInventory();
    // ItemInstance* getItemInstance(Id instance_id);
    void teleportTo(const Position& pos);

    std::string getUsername();
    const Item* removeItemInventory(TypeItem type_item);


    // uint8_t getSlotOfInstance(Id instance_id) const;
    // void increaseInventoryGold(uint32_t amount);
    // void decreaseInventoryGold(uint32_t amount);
    // void increaseBankGold(uint32_t amount);
    // void decreaseBankGold(uint32_t amount);
    // size_t getBankSize() const;
    // size_t getMaxBankSize() const;
    // bool hasItemInBank(Id instance_id) const;
    // std::vector<MsgItemInfo> getBankItemsInfo() const;

    bool isNewbie() const;
    bool isValidOpponent(Player* other) const;
    bool isMeditating() const;
    bool isResurrecting() const;
    bool hasEnoughMana(uint16_t mana_cost) const;

    void addItemToInventory(const ItemInstance& instance);
    void addItemToInventory(const GoldBagInstance& instance);
    void addItemToInventory(const TreasureInstance& instance);

    void toggleMeditation();
    void breakMeditation();
    void updateHp(float delta);
    void updateMana(float delta);
    void meditating(float delta);
    void restoreAllMana();
    // void restoreMana(uint16_t amount);
    void restoreAllHp();
    void restoreHp(uint16_t amount);
    void startResurrection();
    void finishResurrection();
    void earnExperiencePoints(CombatEntity* victim, uint16_t damage);

    void buyItem(const ShopItem* item);
    void sellItem(TypeItem type_item, uint32_t sell_price);
    void dropItem(size_t index, World& world);

    // void addItemToInventory(std::unique_ptr<ItemInstance> item);
    // std::unique_ptr<ItemInstance> removeItemFromInventory(Id instance_id);
    // void addItemToBank(std::unique_ptr<ItemInstance> item);
    // std::unique_ptr<ItemInstance> removeItemFromBank(Id instance_id);

    // void equipItem(Id instance_id);
    // void unequipItem(Id instance_id);
    // void useItem(Id instance_id);

    uint16_t calculateDamage(bool& is_critical, Weapon& weapon);
    uint16_t calculateDefense(std::vector<Defense*> info_defense);

    PlayerSnapshotData getPlayerSnapshotData(const Id& id);
    // MsgPlayerStats getPlayerStats();
    PlayerData getPlayerData();

    void onDeath(World& world) override;  // El jugador se convierte en fantasma

    ~Player() override = default;
};


#endif
