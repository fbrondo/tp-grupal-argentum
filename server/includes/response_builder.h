#ifndef RESPONSE_BUILDER_H
#define RESPONSE_BUILDER_H
#include <map>
#include <memory>

#include "server/includes/core/snapshot.h"
#include "server/includes/player.h"

#include "world.h"

class ResponseBuilder {
private:
    /* data */
public:
    ResponseBuilder(/* args */) = default;
    Snapshot buildSnapshot(
        const std::map<Id, std::unique_ptr<Player>>& players,
        const std::map<Id, NpcInstance>& npcs,
        const std::map<Id, NpcInstance>& creatures,
        const std::map<Id, ItemInstance>& items_on_floor,
        const std::map<Id, GoldBagInstance>& gold_on_floor,
        const std::vector<SoundEffectSnapshotData>& sound_effects);
    ~ResponseBuilder();
};

#endif
