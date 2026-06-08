#include "server/includes/response_builder.h"

#include "server/includes/core/instances.h"
Snapshot ResponseBuilder::buildSnapshot(const std::map<Id, std::unique_ptr<Player>>& /*pls*/,
                                        World& /*world*/) {
    Snapshot snap;
    // for (auto& [id, player]: players) {
    //     // const Pose& pos = world.positionPlayerInTheWorld(id);
    //     // PlayerSnapshotData p;
    //     // p.id = static_cast<uint32_t>(id);
    //     // p.pos_x = pos.x;
    //     // p.pos_y = pos.y;
    //     // p.direction = inst.direct;
    //     // snap.players.push_back(p);
    // }
    return snap;
}

ResponseBuilder::~ResponseBuilder() = default;
