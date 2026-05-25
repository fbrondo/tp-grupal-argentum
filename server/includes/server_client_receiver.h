#pragma once

#include "common/includes/queue.h"
#include "common/includes/protocol.h"
#include "common/includes/thread.h"

#include "server_protocol.h"
#include "snapshot.h"

#include <memory>
#include <stdexcept>

class ClientReceiver : public Thread {
    private:
        uint32_t player_id;
        ServerProtocol& protocol;
        Queue<std::unique_ptr<Command>>& commands_queue;
        Queue<Snapshot>& outgoing_queue;
        
    public:
        ClientReceiver(uint32_t player_id, ServerProtocol& protocol,
                       Queue<std::unique_ptr<Command>>& commands_queue,
                       Queue<Snapshot>& outgoing_queue);
        void run() override;
};
