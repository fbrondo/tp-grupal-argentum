#ifndef STATICS_H
#define STATICS_H

#include <cstdint>

struct Statistics {
    uint16_t intelligence;
    uint16_t constitution;
    uint16_t strength;
    uint16_t agility;

    Statistics(): intelligence(0), constitution(0), strength(0), agility(0) {}
    Statistics(uint16_t intel, uint16_t cons, uint16_t strg, uint16_t agil):
            intelligence(intel), constitution(cons), strength(strg), agility(agil) {}
    ~Statistics() = default;
};

#endif
