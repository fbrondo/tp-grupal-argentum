#ifndef TALLER_TP_NPCS_POSITIONS_H
#define TALLER_TP_NPCS_POSITIONS_H

#include <unordered_map>

#include "common//includes/types.h"
#include "common/includes/core/position.h"
#include "server/includes/core/instances.h"
#include "server/includes/core/map.h"

class NpcPositions {
private:
    std::unordered_map<Id, NpcInstance> priests;
    std::unordered_map<Id, NpcInstance> creatures;
    std::unordered_map<Id, NpcInstance> merchants;
    std::unordered_map<Id, NpcInstance> bankers;
    std::unordered_map<Position, bool, PositionHash> npc_tiles;

public:
    NpcPositions() = default;
    void add(const NpcInstance& npc);
    NpcInstance removeCreature(Id id);
    bool isOcupied(const Position& position) const;
    // get(uint32_t id, uint8_t type);
};

#endif  // TALLER_TP_NPCS_POSITIONS_H
