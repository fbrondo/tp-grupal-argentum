#include "server/includes/game_formulas.h"

#include <cmath>
#define EXP 1.1
namespace GameFormulas {
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
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<uint16_t> dist(minDamage, maxDamage);
    uint16_t final_damage = strength * dist(rng);
    /*Golpe critico?*/
    std::uniform_int_distribution<int> crit_dist(1, 100);
    if (crit_dist(rng) <= 10) {
        is_critical = true;
        final_damage *= 2;
    } else {
        is_critical = false;
    }
    return final_damage;
}

uint16_t calculationPointsExpAttack(const uint16_t& damage, const uint8_t& opponentsLevel,
                                    const uint8_t& victimLevel) {
    return damage * std::max(opponentsLevel - victimLevel + 10, 0);
}

uint16_t calculationPointsExpKill(const uint16_t& opponentsLifeMax, const uint16_t& opponentsLevel,
                                  const uint16_t& level) {
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_real_distribution<double> dist(0, 0.1);
    double rand = dist(rng);
    return static_cast<uint16_t>(rand * opponentsLifeMax *
                                 std::max(opponentsLevel - level + 10, 0));
    // return dist(this->rng) * opponentsLifeMax * std::max(opponentsLevel - level + 10, 0);
}

uint16_t limitMoveUpToNextLevel(const uint8_t& level) { return 1000 * std::pow(level, 1.8); }

uint16_t calculationGoldenMax(const uint16_t& levelPlayer) {
    return 100 * std::pow(levelPlayer, EXP);
}

uint16_t calculationGoldenNpcKill(const uint16_t& lifeMaxNpc) {
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<uint16_t> dist(0, 20);
    uint16_t porcentaje = dist(rng);
    return (lifeMaxNpc * porcentaje) / 100;
}

uint16_t calculationDefense(const std::vector<Defense*>& info_defense) {
    std::random_device rd;
    std::mt19937 rng(rd());
    uint16_t defensa_total = 0;
    for (const Defense* item_defense: info_defense) {
        uint16_t min_def = item_defense->minimal_defense;
        uint16_t max_def = item_defense->maximun_defense;
        std::uniform_int_distribution<uint16_t> dist(min_def, max_def);
        defensa_total += dist(rng);
    }
    return defensa_total;
}

bool calculationDodge() {
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<uint16_t> dist(1, 100);
    uint16_t dodge_chance = 10;
    return dist(rng) <= dodge_chance;
}
}  // namespace GameFormulas
// std::mt19937& getRng() { return this->rng; }
