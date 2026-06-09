#include "server/includes/response_builder.h"

Snapshot ResponseBuilder::buildSnapshot(const std::map<Id, Player>& players, World& world) {
    Snapshot snap;
    for (auto& [id, player]: players) {
        const Position pos = world.positionPlayerInTheWorld(id);
        PlayerSnapshotData p;
        p.id = static_cast<uint32_t>(id);
        p.pos_x = pos.x;
        p.pos_y = pos.y;
        p.direction = static_cast<uint8_t>(player.getDirection());
        p.hp = player.getHp();
        p.max_hp = 0;
        p.mana = player.getMana();
        p.max_mana = 0;
        p.body_id = player.getBodyId();
        p.head_id = player.getHeadId();
        p.weapon_id = 0;
        p.shield_id = 0;
        p.raza = static_cast<uint8_t>(player.getRace());
        p.clase = static_cast<uint8_t>(player.getClase());
        p.flags = 0;
        snap.players.push_back(p);
    }
    return snap;
}

ResponseBuilder::~ResponseBuilder() = default;
