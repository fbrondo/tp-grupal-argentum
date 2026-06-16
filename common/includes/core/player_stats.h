#ifndef PLAYER_STATS_H
#define PLAYER_STATS_H

#include <cstdint>

#pragma pack(push, 1)
struct PlayerStats  {
    uint8_t level;
    uint16_t xp;         // o experience

    uint16_t current_hp;  // o simplemente hp
    uint16_t max_hp;

    uint16_t current_mana; // o simplemente mana
    uint16_t max_mana;
};
#pragma pack(pop)
#endif //PLAYER_STATS_H
