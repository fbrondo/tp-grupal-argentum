#include "server/includes/responses/response_snapshot.h"
#include "server/includes/server_protocol.h"

ResponseSnapshot::ResponseSnapshot(Snapshot&& snp): snap(std::move(snp)) {}

void ResponseSnapshot::execute(ServerProtocol& protocol) { protocol.sendSnapshot(this->snap); }
