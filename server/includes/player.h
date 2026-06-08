#ifndef PLAYER_H
#define PLAYER_H

#include <cstdint>
#include <map>
#include <memory>
#include <utility>
#include <vector>

#include "common/includes/core/user.h"
#include "common/includes/direction.h"
#include "common/includes/protocol.h"
#include "common/includes/types.h"
#include "entity/combat_entity.h"
#include "server/includes/character.h"
#include "server/includes/core/config.h"
#include "server/includes/core/data.h"
#include "server/includes/core/inventory.h"
#include "server/includes/core/map.h"
#include "server/includes/equipment.h"

struct BankAccount {
    uint32_t gold;
    std::map<Id, std::unique_ptr<ItemInstance>> safe_box;
    size_t max_size{30};
};

class Player: public CombatEntity {
private:
    bool is_meditating = false;
    uint16_t mana;
    uint16_t exp;

    User user;
    Inventory inv;
    Equipment equipment;
    Character ch;
    Statistics statics;
    BankAccount bank_account;

    uint16_t hpMax();
    uint16_t manaMax();

public:
    Player(const Player& other) = delete;
    Player& operator=(const Player& other) = delete;
    Player(Player&&) = default;
    Player& operator=(Player&&) = default;

    explicit Player(const Pose& pos_, Inventory&& inv_, Character&& ch_, const PlayerData& data);
    /*Constructor para un jugador registrado desde cero - nuevo */
    explicit Player(const User& user, const Pose& pose, Character&& ch,
                    const PlayerStateInit& state_init);

    TypeItem getHandItem();
    std::vector<TypeItem> getEquipment();
    Inventory& getInventory();
    ItemInstance* getItemInstance(Id instance_id);
    Pose getPose() const;

    uint32_t getInventorySize() const;
    uint32_t getMaxInventorySize() const;
    uint32_t getInventoryGold() const;
    uint32_t getBankGold() const;

    void increaseInventoryGold(uint32_t amount);
    void decreaseInventoryGold(uint32_t amount);
    void increaseBankGold(uint32_t amount);
    void decreaseBankGold(uint32_t amount);

    size_t getBankSize() const;
    size_t getMaxBankSize() const;
    bool hasItemInBank(Id instance_id) const;
    std::vector<MsgItemInfo> getBankItemsInfo() const;

    bool isNewbie() const;
    bool isValidOpponent(Player* other) const;
    bool isMeditating() const;
    bool hasEnoughMana(uint16_t mana_cost) const;

    void toggleMeditation();
    void breakMeditation();
    void updateHp(float delta);
    void updateMana(float delta);
    void meditating(float delta);
    void restoreAllMana();
    void restoreAllHp();
    void earnExperiencePoints(CombatEntity* victim, uint16_t damage);

    // void updatePose(Pose&& new_pos);

    bool canBuy(const ShopItem* item) const;
    void buyItem(const ShopItem* item, Id id_instance);
    bool canSell(Id instance_id) const;
    void sellItem(Id instance_id, uint32_t sell_price);
    void addItemToInventory(std::unique_ptr<ItemInstance> item);
    std::unique_ptr<ItemInstance> removeItemFromInventory(Id instance_id);
    void addItemToBank(std::unique_ptr<ItemInstance> item);
    std::unique_ptr<ItemInstance> removeItemFromBank(Id instance_id);

    uint16_t calculateDamage(bool& is_critical, Weapon& weapon);
    uint16_t calculateDefense(std::vector<Defense*> info_defense);


    PlayerData getPlayerData();

    void onDeath(World& world) override;  // El jugador se convierte en fantasma

    ~Player() override = default;
};


#endif
