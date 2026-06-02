#include "../includes/monitor_queues.h"

#include <algorithm>
#include <ranges>
#include <utility>

MonitorQueues::MonitorQueues() {}

QueueResp& MonitorQueues::addQueuePlayer(const Id& player_id) {
    std::lock_guard lock(this->mut);
    QueueResp new_queue;
    this->queues_players.emplace(player_id, std::move(new_queue));
    return queues_players.at(player_id);
}
void MonitorQueues::queueTheServerResponse(const Id& player_id, std::unique_ptr<Response>&& response_server) {
    this->queues_players[player_id].try_push(std::move(response_server));
}

void MonitorQueues::executeBroadcast(std::unique_ptr<ResponseSnapshot>&& response_snapshot) {
    std::lock_guard lock(this->mut);
    for (auto& queue: this->queues_players | std::views::values) {
        try {
            queue.try_push(std::move(response_snapshot));
        } catch (const ClosedQueue&) {}
    }
}

void MonitorQueues::removeQueuesPlayer(const Id& player_id) {
    std::lock_guard lock(this->mut);
    this->queues_players.erase(player_id);
}
