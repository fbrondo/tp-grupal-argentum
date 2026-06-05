#ifndef PLAYER_H
#define PLAYER_H

#include <cstdint>
#include <memory>
#include <string>

#include "common/includes/direction.h"
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

class Player: public CombatEntity {
private:
    uint16_t mana;
    uint16_t exp;
    uint8_t level;

    User user;
    // Pose pose;
    Inventory inv;
    Equipment equipment;
    Character ch;
    Statistics statics;

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
    ItemInstance *getItemInstance(Id instance_id);

    bool hasEnoughMana(uint16_t mana_cost) const;
    void updatePose(Position pos, Direction direct);

    bool canBuy(const Item* item) const;
    void buyItem(const Item* item, Id id_instance);
    bool canSell(Id instance_id) const;
    void sellItem(Id instance_id, uint32_t sell_price);

    uint16_t calculateDamage(bool& is_critical, Weapon& weapon);
    uint16_t calculateDefense(std::vector<Defense*> info_defense);

    /*El jugador nos devuelve los datos que seran guardados*/
    PlayerData getPlayerData();

    void onDeath() override;  // El jugador se convierte en fantasma

    ~Player() override = default;
};


#endif
