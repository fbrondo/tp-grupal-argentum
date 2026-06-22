#pragma once

#include <cstdint>
#include <random>
#include <vector>

#include "common/includes/core/snapshot.h"

class EffectManager {
private:
    std::mt19937 gen{std::random_device{}()};
    std::vector<SoundEffectSnapshotData> sounds;
    std::vector<VisualEffectSnapshotData> visuals;

public:
    void emitSound(SoundEffectID effect_id, const Position& position);
    void emitRandomSound(uint16_t first_id, uint16_t last_id, const Position& position);
    void emitVisual(VisualEffectID effect_id, const Position& position, Id recipient_id = 0);
    void flushInto(Snapshot& snapshot);
};
