#ifndef STATICS_H
#define STATICS_H

#include <cstdint>

struct Statics {
    uint16_t intelligense;
    uint16_t constitution;
    uint16_t strength;
    uint16_t agility;

    Statics() = default;
    Statics(uint16_t intel, uint16_t cons, uint16_t strg, uint16_t agil):
            intelligense(intel), constitution(cons), strength(strg), agility(agil) {}
    virtual ~Statics() = default;
};

#endif
