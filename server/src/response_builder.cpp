#include "server/includes/response_builder.h"
#include "server/includes/core/instances.h"
Snapshot ResponseBuilder:: buildSnapshot(const std::map<Id,Player>& players, World& world) {
    Snapshot snap;
    for (auto& [id, player]: players) {
        const PlayerInstance& inst = world.playerInformationInTheWorld(id);
        PlayerSnapshotData p;
        p.id = static_cast<uint32_t>(id);
        p.pos_x = inst.position.x;
        p.pos_y = inst.position.y;
        p.direction = inst.direct;
        snap.players.push_back(p);
    }
    return snap;
}

ResponseBuilder::~ResponseBuilder()
{
}
