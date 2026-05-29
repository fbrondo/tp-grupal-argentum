#ifndef PLAYER_H
#define PLAYER_H

#include <cstdint>
#include <map>
#include <memory>
#include <string>

#include "common/includes/types.h"
#include "core/item.h"
#include "core/instances.h"
#include "core/map.h"

#include "character.h"
#include "equipment.h"
#include "game_formulas.h"

struct User {
    std::string username;
    std::string password;
};

class Player {
private:
    uint16_t hp;
    uint16_t mana;
    uint16_t exp;
    uint16_t level;

    //const Position& pos;
    Equipment equipment;
    Character ch;
    GameFormulas form;

    /*Agrupoar en un strutc - Inventory*/
    uint16_t golden;
    std::map<Id, std::unique_ptr<ItemInstace>> inventory;


    uint16_t hpMax(const uint16_t& constitution);
    uint16_t manaMax(const uint16_t& intelligense);

public:
    Player(const Player& other) = delete;
    Player& operator=(const Player& other) = delete;
    
    
    Player(Player&&) = default;
    Player& operator=(Player&&) = default;

    Player(const Race& ch_race, const Clase& ch_clase, uint16_t golden, uint8_t level /*const Position& pos*/);

    bool isAlive();
    //const Position& getCurrentPosition() const;
    //void updatePosition(Position&& new_pos);

    /*tirar item*/
    /*equipar item*/
    /*recibir dano*/
    /*usarItem*/

    ~Player();
};


#endif
