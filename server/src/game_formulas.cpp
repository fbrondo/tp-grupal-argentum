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
                                         const uint16_t& maxDamage, bool& is_critical) {
    std::uniform_int_distribution<uint16_t> dist(minDamage, maxDamage);
    uint16_t final_damage = strength * dist(this->rng);

    // Golpe critico?
    std::uniform_int_distribution<int> crit_dist(1, 100);
    if (crit_dist(this->rng) <= 10) {
        is_critical = true;
        final_damage *= 2;
    } else {
        is_critical = false;
    }

    return final_damage; 
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
    std::uniform_int_distribution<uint16_t> dist(0, 20);
    uint16_t porcentaje = dist(this->rng);

    return (lifeMaxNpc * porcentaje) / 100;
}

uint16_t GameFormulas::calculationDefense(
        const std::vector<const ItemInstance*>& equipment_def,
        const std::map<TypeItem, std::unique_ptr<Item>>& info_items) {
    uint16_t defensa_total = 0;

    for (const ItemInstance* item_inst: equipment_def) {
        TypeItem type = item_inst->type;
        const Item& item_template = *(info_items.at(type));

        auto defense_data = dynamic_cast<const Defense*>(&item_template);

        if (defense_data) {
            uint16_t min_def = defense_data->minimal_defense;
            uint16_t max_def = defense_data->maximun_defense;

            if (max_def > 0) {
                std::uniform_int_distribution<uint16_t> dist(min_def, max_def);
                defensa_total += dist(this->rng);
            }
        }
    }

    return defensa_total;
}

bool GameFormulas::calculationDodge() {
    std::uniform_int_distribution<uint16_t> dist(1, 100);

    uint16_t dodge_chance = 10;

    return dist(this->rng) <= dodge_chance;
}

std::mt19937& GameFormulas::getRng() { return this->rng; }
