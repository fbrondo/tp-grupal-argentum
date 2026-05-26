#include "../includes/game_formulas.h"

#include <cmath>

#define EXP 1.1

uint16_t GameFormulas::calculationMaximunHp(const uint16_t& constitution,
                                            const uint16_t& hpFactorRace,
                                            const uint16_t& hpFactorClase, const uint8_t& level) {
    return constitution * hpFactorRace * hpFactorClase * level;
}

uint16_t GameFormulas::calculationRecoveredHp(const uint16_t& recoveryFactor, float delta) {
    return recoveryFactor * delta;
}

uint16_t GameFormulas::calculationMaximunMana(const uint16_t& intelligense,
                                              const uint16_t& manaFactorRace,
                                              const uint16_t& manaFactorClase,
                                              const uint8_t& level) {
    return intelligense * manaFactorRace * manaFactorClase * level;
}

uint16_t GameFormulas::calculationOfManaTime(const uint16_t& recoveryFactor, float delta) {
    return recoveryFactor * delta;
}

uint16_t GameFormulas::calculationOfManaMeditation(const uint16_t& meditationFactorClase,
                                                   const uint16_t& intelligense, float delta) {
    return meditationFactorClase * intelligense * delta;
}

uint16_t GameFormulas::calculationDamage(const uint16_t& strength, const uint16_t& minDamage,
                                         const uint16_t& maxDamage) {
    std::uniform_int_distribution<uint16_t> dist(minDamage, maxDamage);
    return strength * dist(this->rng);
}

uint16_t GameFormulas::calculationPointsExpAttack(const uint16_t& damage,
                                                  const uint8_t& opponentsLevel,
                                                  const uint8_t& victimLevel) {
    return damage * std::max(opponentsLevel - victimLevel + 10, 0);
}

uint16_t GameFormulas::calculationPointsExpKill(const uint16_t& opponentsLifeMax,
                                                const uint16_t& opponentsLevel,
                                                const uint16_t& level) {
    std::uniform_int_distribution<uint16_t> dist(0, 0.1);
    return dist(this->rng) * opponentsLifeMax * std::max(opponentsLevel - level + 10, 0);
}

uint16_t GameFormulas::limitMoveUpToNextLevel(const uint8_t& level) {
    return 1000 * std::pow(level, 1.8);
}

uint16_t GameFormulas::calculationGoldenMax(const uint16_t& levelPlayer) {
    return 100 * std::pow(levelPlayer, EXP);
}

uint16_t GameFormulas::calculationGoldenNpcKill(const uint16_t& lifeMaxNpc) {
    std::uniform_int_distribution<uint16_t> dist(0, 0.2);
    return dist(this->rng) * lifeMaxNpc;
}
