#include "../includes/monitor_queues.h"

#include <algorithm>
#include <utility>

MonitorQueues::MonitorQueues() {}

Queue<Snapshot>& MonitorQueues::addQueuePlayer(const Id& id_player) {
    std::lock_guard<std::mutex> lock(this->mut);
    this->queues_players.emplace(std::piecewise_construct, std::forward_as_tuple(id_player),
                                 std::forward_as_tuple());
    return queues_players.at(id_player);
}

void MonitorQueues::executeBroadcast(const Snapshot& message) {
    std::lock_guard<std::mutex> lock(this->mut);
    for (auto& [id, queue]: this->queues_players) {
        try {
            queue.try_push(message);
        } catch (const ClosedQueue&) {}
    }
}

void MonitorQueues::removeQueuesPlayer(const Id& id_player) {
    std::lock_guard<std::mutex> lock(this->mut);
    // this->queues_players[id_player].close();
    this->queues_players.erase(id_player);
}
