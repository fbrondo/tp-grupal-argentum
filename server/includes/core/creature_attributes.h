#ifndef CRIATURE_ATTRIBUTES_H
#define CRIATURE_ATTRIBUTES_H

#include <cstdint>

#pragma pack(push, 1)
struct NpcAttributes {
    uint16_t max_hp{0};
    uint16_t current_hp{0};
    uint16_t difficulty_level{0};
    uint16_t range_attack{0};

    NpcAttributes() = default;
    NpcAttributes(NpcAttributes&&) = default;
    NpcAttributes& operator=(NpcAttributes&&) = default;

    NpcAttributes(const NpcAttributes&) = default;
    NpcAttributes& operator=(const NpcAttributes&) = default;
};
#pragma pack(pop)
#endif  // TALLER_TP_CRIATURE_ATTRIBUTES_H
