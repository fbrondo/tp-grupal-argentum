#ifndef PLAYER_H
#define PLAYER_H

#include <cstdint>
#include <map>
#include <memory>
#include <string>

#include "common/includes/direction.h"
#include "common/includes/types.h"
#include "server/includes/character.h"
#include "server/includes/core/combat_entity.h"
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

class Player: public CombatEntity {
private:
    uint16_t mana;
    uint16_t exp;
    uint8_t level;

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

    Equipment& getEquipment();
    uint16_t calculateDamage(bool& is_critical,
                             const std::map<TypeItem, std::unique_ptr<Item>>& info_items);

    void receiveDamage(uint16_t damage,
                       const std::map<TypeItem, std::unique_ptr<Item>>& info_items) override;
    void onDeath() override;  // El jugador se convierte en fantasma

    ~Player() override = default;
};

#endif
