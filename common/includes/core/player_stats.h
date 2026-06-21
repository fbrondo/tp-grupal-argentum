#ifndef PLAYER_STATS_H
#define PLAYER_STATS_H

#include <cstdint>

#pragma pack(push, 1)
struct PlayerStats {
    uint8_t level;
    uint32_t xp;
    uint16_t current_hp;
    uint16_t max_hp;
    uint16_t current_mana;
    uint16_t max_mana;
    uint32_t gold;
};
#pragma pack(pop)
#endif  // PLAYER_STATS_H
