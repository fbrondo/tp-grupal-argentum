#include "server/includes/responses/response_player_stats.h"

#include "server/includes/server_protocol.h"

ResponsePlayerStats::ResponsePlayerStats(const MsgPlayerStats& stats): stats(stats) {}

void ResponsePlayerStats::execute(ServerProtocol& protocol) {
    protocol.sendPlayerStats(this->stats);
}
