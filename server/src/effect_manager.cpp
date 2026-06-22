#include "server/includes/effect_manager.h"

#include <utility>

void EffectManager::emitSound(SoundEffectID effect_id, const Position& position) {
    sounds.push_back({effect_id, position.x, position.y});
}

void EffectManager::emitRandomSound(uint16_t first_id, uint16_t last_id, const Position& position) {
    std::uniform_int_distribution<uint16_t> dist(first_id, last_id);
    emitSound(static_cast<SoundEffectID>(dist(gen)), position);
}

void EffectManager::emitVisual(VisualEffectID effect_id, const Position& position,
                               Id recipient_id) {
    visuals.push_back({effect_id, recipient_id, position.x, position.y});
}

void EffectManager::flushInto(Snapshot& snapshot) {
    snapshot.sound_effects = std::move(sounds);
    snapshot.visual_effects = std::move(visuals);
    sounds.clear();
    visuals.clear();
}
