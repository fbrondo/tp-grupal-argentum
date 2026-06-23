#include "server/includes/game_formulas.h"

#include <cmath>
#define EXP 1.1
namespace GameFormulas {
namespace {
std::mt19937& getRng() {
    static std::mt19937 rng{std::random_device{}()};
    return rng;
}
}  // namespace
uint16_t calculationMaximunHp(const uint16_t& constitution, const uint16_t& hpFactorRace,
                              const uint16_t& hpFactorClase, const uint8_t& level) {
    return constitution * hpFactorRace * hpFactorClase * level;
}

uint16_t calculationRecoveredHp(const uint16_t& recoveryFactor, float delta) {
    return recoveryFactor * delta;
}

uint16_t calculationMaximunMana(const uint16_t& intelligense, const uint16_t& manaFactorRace,
                                const uint16_t& manaFactorClase, const uint8_t& level) {
    return intelligense * manaFactorRace * manaFactorClase * level;
}

uint16_t calculationOfManaTime(const uint16_t& recoveryFactor, float delta) {
    return recoveryFactor * delta;
}

uint16_t calculationOfManaMeditation(const uint16_t& meditationFactorClase,
                                     const uint16_t& intelligense, float delta) {
    return meditationFactorClase * intelligense * delta;
}

uint16_t calculationDamage(const uint16_t& strength, const uint16_t& minDamage,
                           const uint16_t& maxDamage, bool& is_critical) {
    std::uniform_int_distribution<uint16_t> dist(minDamage, maxDamage);
    uint16_t final_damage = strength * dist(getRng());
    /*Golpe critico?*/
    std::uniform_int_distribution<int> crit_dist(1, 100);
    if (crit_dist(getRng()) <= 10) {
        is_critical = true;
        final_damage *= 2;
    } else {
        is_critical = false;
    }
    return final_damage;
}

uint32_t calculationPointsExpAttack(const uint16_t& damage, const uint8_t& opponentsLevel,
                                    const uint8_t& level) {
    return static_cast<uint32_t>(damage) *
           std::max(static_cast<int>(opponentsLevel) - static_cast<int>(level) + 10, 0);
}

uint32_t calculationPointsExpKill(const uint16_t& opponentsLifeMax, const uint8_t& opponentsLevel,
                                  const uint8_t& level) {
    std::uniform_real_distribution<float> dist(0, 0.1f);
    double rand = dist(getRng());
    int result = rand * opponentsLifeMax *
                 std::max(static_cast<int>(opponentsLevel) - static_cast<int>(level) + 10, 0);
    return static_cast<uint32_t>(result);
}

uint32_t limitMoveUpToNextLevel(const uint8_t& level) {
    return static_cast<uint32_t>(1000 * std::pow(level, 1.8));
}

uint16_t calculationGoldenMax(const uint8_t& levelPlayer) {
    return 100 * std::pow(levelPlayer, EXP);
}

uint32_t calculationGoldenNpcKill(const uint16_t& lifeMaxNpc) {
    std::uniform_real_distribution<float> dist(0.01f, 0.2f);
    float multiplicador = dist(getRng());
    return lifeMaxNpc * multiplicador;
}

uint16_t calculationDefense(const std::vector<Defense*>& info_defense) {
    uint16_t defensa_total = 0;
    for (const Defense* item_defense: info_defense) {
        uint16_t min_def = item_defense->minimal_defense;
        uint16_t max_def = item_defense->maximun_defense;
        std::uniform_int_distribution<uint16_t> dist(min_def, max_def);
        defensa_total += dist(getRng());
    }
    return defensa_total;
}

bool calculationDodge() {
    std::uniform_int_distribution<uint16_t> dist(1, 100);
    uint16_t dodge_chance = 10;
    return dist(getRng()) <= dodge_chance;
}
}  // namespace GameFormulas
