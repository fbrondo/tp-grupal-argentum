#ifndef MONITORQUEUES_H
#define MONITORQUEUES_H

#include <mutex>
#include <unordered_map>

#include "common/includes/queue.h"
#include "common/includes/types.h"

#include "definitions.h"
#include "snapshot.h"

class MonitorQueues {

private:
    std::mutex mut;
    MapQueues queues_players;

public:
    MonitorQueues(const MonitorQueues& other) = delete;
    MonitorQueues& operator=(const MonitorQueues& other) = delete;

    MonitorQueues();
    Queue<Snapshot>& addQueuePlayer(const Id& id_player);
    void executeBroadcast(const Snapshot& message);
    void removeQueuesPlayer(const Id& id_player);
};

#endif
