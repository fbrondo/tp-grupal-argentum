#include "server/includes/response_builder.h"

Snapshot ResponseBuilder::buildSnapshot(const std::map<Id, Player>& players, World& world) {
    Snapshot snap;
    for (auto& [id, player]: players) {
        const Position pos = world.positionPlayerInTheWorld(id);
        PlayerSnapshotData p;
        p.id = static_cast<uint32_t>(id);
        p.pos_x = pos.x;
        p.pos_y = pos.y;
        p.direction = 0;
        p.body_id = BROWN_SPIKY_BEARD;
        p.head_id = CITIZEN_LEATHER;
        snap.players.push_back(p);
    }
    return snap;
}

ResponseBuilder::~ResponseBuilder() = default;
