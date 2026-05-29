#include "../includes/monitor_queues.h"

#include <algorithm>
#include <utility>

MonitorQueues::MonitorQueues() {}

QueueResp& MonitorQueues::addQueuePlayer(const Id& player_id) {
    std::lock_guard<std::mutex> lock(this->mut);
    QueueResp new_queue;
    this->queues_players.emplace(std::move(new_queue)/*std::piecewise_construct, std::forward_as_tuple(player_id), std::forward_as_tuple()*/);
    return queues_players.at(player_id);
}
void MonitorQueues::queueTheServerResponse(const Id& player_id, std::unique_ptr<Response>&& response_server) {
    this->queues_players[player_id].try_push(std::move(response_server));
}

void MonitorQueues::executeBroadcast(const Snapshot snapsh) {
    std::lock_guard<std::mutex> lock(this->mut);
    for (auto& [id, queue]: this->queues_players) {
        try {
            //queue.try_push(response_snapshot);
        } catch (const ClosedQueue&) {}
    }
}

void MonitorQueues::removeQueuesPlayer(const Id& player_id) {
    std::lock_guard<std::mutex> lock(this->mut);
    // this->queues_players[player_id].close();
    this->queues_players.erase(player_id);
}
