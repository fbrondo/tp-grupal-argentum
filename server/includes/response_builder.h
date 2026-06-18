#ifndef RESPONSE_BUILDER_H
#define RESPONSE_BUILDER_H
#include <map>
#include <memory>
#include <vector>

#include "common/includes/core/snapshot.h"
#include "server/includes/npc/citizen_npc.h"
#include "server/includes/npc/creature.h"
#include "server/includes/player.h"

#include "world.h"

namespace ResponseBuilder {
// std::vector<PlayerSnapshotData> buildPlayerSnapshot(const std::map<Id, std::unique_ptr<Player>>&
// players);
std::vector<PlayerSnapshotData> buildPlayerSnapshot(
        const std::map<Id, std::unique_ptr<Player>>& players);
std::vector<NpcSnapshotData> buildNpcSnapshot(const std::map<Id, std::unique_ptr<Creature>>& npcs);
std::vector<ItemGroundSnapshotData> buildGoldPileSnapshot(const std::map<Id, TypeItem>& items);

std::vector<CitizenNpcSnapshot> buildCitizenNpcSnapshot(
        const std::map<Id, std::unique_ptr<CitizenNPC>>& citizens);

};  // namespace ResponseBuilder

#endif
