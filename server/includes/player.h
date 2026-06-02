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
#include "server/includes/core/instances.h"
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

    Player(Inventory&& inv_, const Race& ch_race, const Clase& ch_clase, uint8_t level);

    /*Nuevo jugador registrado desde cero*/
    Player(const Race& race, const Clase& clase, const PlayerStateInitConfig& state_init);

    bool isAlive();
    // const Position& getCurrentPosition() const;
    // void updatePosition(Position&& new_pos);

    /*tirar item*/
    /*equipar item*/
    /*recibir dano*/
    /*usarItem*/

    ~Player();
};


#endif
