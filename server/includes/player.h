#ifndef PLAYER_H
#define PLAYER_H

#include <cstdint>
#include <memory>
#include <string>

#include "common/includes/direction.h"
#include "common/includes/types.h"
#include "server/includes/character.h"
#include "server/includes/core/combat_entity.h"
#include "server/includes/core/config.h"
#include "server/includes/core/data.h"
#include "server/includes/core/inventory.h"
#include "server/includes/core/map.h"
#include "server/includes/equipment.h"
#include "server/includes/game_formulas.h"

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
    Pose pose;
    Inventory inv;
    Equipment equipment;
    Character ch;

    uint16_t hpMax(const uint16_t& constitution);
    uint16_t manaMax(const uint16_t& intelligense);

public:
    Player(const Player& other) = delete;
    Player& operator=(const Player& other) = delete;
    Player(Player&&) = default;
    Player& operator=(Player&&) = delete;

    Player(Id id, Position&& pos, Inventory&& inv_, const Race& race, const Clase& clase,
           uint8_t level, GameFormulas& formulas);
    Player(Id id, Position&& pos, const Race& race, const Clase& clase,
           const PlayerStateInitConfig& state_init, GameFormulas& formulas);
    // Player(User&& user, const Position& pos, Inventory&& inv_, const Race& race, const Clase&
    // clase, uint8_t level);

    /*Constructor para un jugador registrado desde cero - nuevo */
    Player(User&& user, Pose&& pose, Character&& ch, const PlayerStateInitConfig& state_init);

    Equipment& getEquipment();
    uint16_t calculateDamage(bool& is_critical,
                             const std::map<TypeItem, std::unique_ptr<Item>>& info_items);
    bool isAlive();

    /*El jugador nos devuelve los datos que seran guardados*/
    PlayerData getPlayerData();
    void updatePose(Position pos, Direction direct);


    void receiveDamage(uint16_t damage,
                       const std::map<TypeItem, std::unique_ptr<Item>>& info_items) override;
    void onDeath() override;  // El jugador se convierte en fantasma

    ~Player() override = default;
};


#endif
