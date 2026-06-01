#include "server/includes/responses/response_snapshot.h"

ResponseSnapshot::ResponseSnapshot(Snapshot&& snp): snap(std::move(snp)){}

void ResponseSnapshot::execute(ServerProtocol& protocol) {
    protocol.sendSnapshot(this->snap);
};