#ifndef PLAYER_H
#define PLAYER_H

#include <cstdint>
#include <map>
#include <memory>
#include <string>

#include "common/includes/direction.h"
#include "common/includes/types.h"
#include "server/includes/character.h"
#include "server/includes/core/config.h"
#include "server/includes/core/data.h"
// #include "server/includes/core/instances.h"
#include "server/includes/core/inventory.h"
#include "server/includes/core/map.h"
#include "server/includes/equipment.h"
#include "server/includes/game_formulas.h"

struct User {
    std::string username;
    std::string password;
};

class Player {
private:
    uint16_t hp;
    uint16_t mana;
    uint16_t exp;
    uint8_t level;
    User user;
    const Position& position;
    Inventory inv;
    Equipment equipment;
    Character ch;
    GameFormulas form;

    uint16_t hpMax(const uint16_t& constitution);
    uint16_t manaMax(const uint16_t& intelligense);

public:
    Player(const Player& other) = delete;
    Player& operator=(const Player& other) = delete;

    Player(Player&&) = default;
    Player& operator=(Player&&) = delete;

    // Player(User&& user, const Position& pos, Inventory&& inv_, const Race& race, const Clase&
    // clase, uint8_t level);

    /*Constructor para un jugador registrado desde cero - nuevo */
    Player(User&& user, const Position& pos, const Race& race, const Clase& clase,
           const PlayerStateInitConfig& state_init);

    bool isAlive();

    /*El jugador nos devuelve los datos que seran guardados*/
    PlayerData getPlayerData();


    /*tirar item*/
    /*equipar item*/
    /*recibir dano*/
    /*usarItem*/

    ~Player();
};


#endif
