#ifndef CHARACTER_TRAITS_H
#define CHARACTER_TRAITS_H

#include <cstdint>

#pragma pack(push, 1)
struct CharacterTraits {
    uint8_t head;
    uint8_t body;
    uint8_t race;
    uint8_t clase;
    CharacterTraits(): head(0), body(0), race(0), clase(0) {}

};
#pragma pack(pop)

#endif  // TALLER_TP_CHARACTER_TRAITS_H
