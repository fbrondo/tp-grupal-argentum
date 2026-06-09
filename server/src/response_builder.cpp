#include "server/includes/response_builder.h"

#include "server/includes/core/instances.h"
namespace ResponseBuilder {

    std::vector<PlayerSnapshotData> buildPlayerSnapshot(const std::map<Id, std::unique_ptr<Player>>& players) {
        std::vector<PlayerSnapshotData> result;
        for (auto& [id, player]: players) {
           PlayerSnapshotData data_player = player->getPlayerSnapshotData(id);
            result.push_back(data_player);
        }
        return result;
    }

    std::vector<NpcSnapshotData> buildNpcSnapshot(const std::map<Id, std::unique_ptr<Creature>> &npcs) {
        std::vector<NpcSnapshotData> result;
        for (auto& [id, npc]: npcs) {
            NpcSnapshotData data_npc = npc->getNpcSnapshotData();
            result.push_back(data_npc);
        }
        return result;
    }
}
