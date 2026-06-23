#include "server/includes/npc/citizen_npc.h"

#include <cstring>

#include "common/includes/protocol.h"
CitizenNPC::CitizenNPC(TypeNPC type, const std::string& name, const Pose& pose, const Id& zone_id):
        type_npc(type), name(name), pose(pose), zone_id(zone_id) {}

TypeNPC CitizenNPC::getTypeNPC() const { return this->type_npc; }

const Position& CitizenNPC::getPosition() const { return this->pose.position; }

CitizenNpcSnapshot CitizenNPC::getSnapshotCitizenNPC(const Id& npc_id) const {
    CitizenNpcSnapshot snapshot;
    std::memset(snapshot.name, 0, MAX_NAME_SIZE);
    this->name.copy(snapshot.name, MAX_NAME_SIZE - 1);
    snapshot.id = npc_id;
    snapshot.position = this->pose.position;
    snapshot.direction = static_cast<uint8_t>(this->pose.direct);
    snapshot.type = static_cast<uint8_t>(this->type_npc);
    return snapshot;
}

CitizenNpcData CitizenNPC::getCitizenNPCData() const {
    CitizenNpcData data;
    std::memset(data.name, 0, MAX_NAME_SIZE);
    this->name.copy(data.name, MAX_NAME_SIZE - 1);
    data.type = static_cast<uint8_t>(this->type_npc);
    data.direction = static_cast<uint8_t>(this->pose.direct);
    data.position = this->pose.position;
    data.zone_id = this->zone_id;
    return data;
}
