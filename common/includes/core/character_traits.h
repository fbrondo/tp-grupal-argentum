#ifndef CHARACTER_TRAITS_H
#define CHARACTER_TRAITS_H

#include <cstdint>

#pragma pack(push, 1)
struct CharacterTraits {
    uint16_t head{0};
    uint16_t body{0};
    uint8_t race{0};
    uint8_t clase{0};
    CharacterTraits() = default;
    CharacterTraits(uint16_t head, uint16_t body, uint8_t race, uint8_t clase):
    head(head), body(body), race(race), clase(clase) {}
};
#pragma pack(pop)

#endif  // TALLER_TP_CHARACTER_TRAITS_H
