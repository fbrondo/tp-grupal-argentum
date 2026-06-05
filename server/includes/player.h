#ifndef PLAYER_H
#define PLAYER_H

#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "common/includes/direction.h"
#include "common/includes/protocol.h"
#include "common/includes/types.h"
#include "server/includes/character.h"
#include "server/includes/core/config.h"
#include "server/includes/core/data.h"
#include "server/includes/core/inventory.h"
#include "server/includes/core/map.h"
#include "server/includes/equipment.h"
#include "server/includes/game_formulas.h"

#include "combat_entity.h"

struct User {
    std::string username;
    std::string password;
    User(std::string&& user, std::string&& pass):
            username(std::move(user)), password(std::move(pass)) {}
};

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
    uint8_t level;

    User user;
    // Pose pose;
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

    Player(Pose&& pos, Inventory&& inv_, const Race& race, const Clase& clase, uint8_t level);
    /*Constructor para un jugador registrado desde cero - nuevo */
    Player(User&& user, Pose&& pose, Character&& ch, const PlayerStateInitConfig& state_init);

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

    bool isMeditating() const;
    void toggleMeditation();
    void breakMeditation();
    bool hasEnoughMana(uint16_t mana_cost) const;
    void restoreAllMana();
    void restoreAllHp();
    void updatePose(Position pos, Direction direct);

    bool canBuy(const Item* item) const;
    void buyItem(const Item* item, Id id_instance);
    bool canSell(Id instance_id) const;
    void sellItem(Id instance_id, uint32_t sell_price);
    void addItemToInventory(std::unique_ptr<ItemInstance> item);
    std::unique_ptr<ItemInstance> removeItemFromInventory(Id instance_id);
    void addItemToBank(std::unique_ptr<ItemInstance> item);
    std::unique_ptr<ItemInstance> removeItemFromBank(Id instance_id);

    uint16_t calculateDamage(bool& is_critical, Weapon& weapon);
    uint16_t calculateDefense(std::vector<Defense*> info_defense);

    /*El jugador nos devuelve los datos que seran guardados*/
    PlayerData getPlayerData();

    void onDeath() override;  // El jugador se convierte en fantasma

    ~Player() override = default;
};


#endif
