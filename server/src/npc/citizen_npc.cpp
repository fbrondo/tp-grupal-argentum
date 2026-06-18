#include "server/includes/npc/citizen_npc.h"

#include <cstring>

#include "common/includes/protocol.h"
CitizenNPC::CitizenNPC(TypeNPC type, const std::string& name, const Pose& pose):
        type_npc(type), name(name), pose(pose) {}

TypeNPC CitizenNPC::getTypeNPC() const { return this->type_npc; }

const Position& CitizenNPC::getPosition() const { return this->pose.position; }

CitizenNpcSnapshot CitizenNPC::getSnapshotCitizenNPC(const Id& npc_id) {
    CitizenNpcSnapshot snapshot;
    std::memset(snapshot.name, 0, MAX_NAME_SIZE);
    this->name.copy(snapshot.name, MAX_NAME_SIZE - 1);
    snapshot.id = npc_id;
    snapshot.position = this->pose.position;
    snapshot.direction = static_cast<uint8_t>(this->pose.direct);
    snapshot.type = static_cast<uint8_t>(this->type_npc);
    return snapshot;
}
