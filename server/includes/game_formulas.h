#ifndef GAME_FORMULAS_H
#define GAME_FORMULAS_H

#include <cstdint>
#include <map>
#include <memory>
#include <random>
#include <vector>

#include "core/instances.h"

class GameFormulas {

private:
    std::mt19937 rng;

public:
    /* (1) Calculo de vida maxima*/
    uint16_t calculationMaximunHp(const uint16_t& constitution, const uint16_t& hpFactorRace,
                                  const uint16_t& hpFactorClase, const uint8_t& level);

    /* (2) Recuperacion de vida por tiempo*/
    uint16_t calculationRecoveredHp(const uint16_t& recoveryFactor, float delta);

    /* (3) Calculo de mana maxima*/
    uint16_t calculationMaximunMana(const uint16_t& intelligense, const uint16_t& manaFactorRace,
                                    const uint16_t& manaFactorClase, const uint8_t& level);

    /* (4) Recuperacion de mana por tiempo*/
    uint16_t calculationOfManaTime(const uint16_t& hpFactorRace, float delta);

    /* (5) Recuperacion de mana por meditacion*/
    uint16_t calculationOfManaMeditation(const uint16_t& meditationFactorClase,
                                         const uint16_t& intelligense, float delta);

    /* (6) Dano de ataque*/
    uint16_t calculationDamage(const uint16_t& strength, const uint16_t& minDamage,
                               const uint16_t& maxlDamage);

    /* (7) Puntos de experiencia por ataque*/
    uint16_t calculationPointsExpAttack(const uint16_t& damage, const uint8_t& opponentsLevel,
                                        const uint8_t& victimLevel);

    /* (8) Puntos de experiencia por muerte de un rival*/
    uint16_t calculationPointsExpKill(const uint16_t& opponentsLifeMax,
                                      const uint16_t& opponentsLevel, const uint16_t& level);

    /* (9) Limite para pasar al siguiente nivel*/
    uint16_t limitMoveUpToNextLevel(const uint8_t& level);

    /* (10) El Oro maximo en mano que tiene un jugador*/
    uint16_t calculationGoldenMax(const uint16_t& levelPlayer);

    /* (11) Oro por matar a un NPC*/
    uint16_t calculationGoldenNpcKill(const uint16_t& lifeMaxNpc);

    /* (12) Puntos por defensa*/
    // uint16_t calculationPointsDefense();
};

#endif
