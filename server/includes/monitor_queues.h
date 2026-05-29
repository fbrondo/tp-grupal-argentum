#ifndef MONITORQUEUES_H
#define MONITORQUEUES_H

#include <mutex>
#include <memory>
#include <unordered_map>
#include <memory>
#include "common/includes/queue.h"
#include "common/includes/types.h"
#include "responses/response.h"

#include "definitions.h"
#include "snapshot.h"

class MonitorQueues {

private:
    std::mutex mut;
    MapQueueResp queues_players;

public:
    MonitorQueues(const MonitorQueues& other) = delete;
    MonitorQueues& operator=(const MonitorQueues& other) = delete;

    MonitorQueues();
    QueueResp& addQueuePlayer(const Id& player_id);

    /*Se guarda la respuesta espeficia para un solo player en su correspondiente cola*/
    void queueTheServerResponse(const Id& player_id, std::unique_ptr<Response>&& response_server);
    void executeBroadcast(const Snapshot snp);
    void removeQueuesPlayer(const Id& player_id);
};

#endif
