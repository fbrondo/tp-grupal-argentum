#ifndef CLIENT_RECEIVER_H
#define CLIENT_RECEIVER_H
// #pragma once

#include <memory>
#include <stdexcept>

#include "common/includes/protocol.h"
#include "common/includes/queue.h"
#include "common/includes/thread.h"
#include "common/includes/types.h"
#include "server/includes/definitions.h"
#include "server/includes/server_protocol.h"

class ClientReceiver: public Thread {
private:
    const Id& player_id;
    ServerProtocol& protocol;
    QueueCmd& commands_queue;
    QueueResp& outgoing_queue;

public:
    ClientReceiver(const Id& player_id, ServerProtocol& protocol, QueueCmd& commands_queue,
                   QueueResp& outgoing_queue);
    ~ClientReceiver();
    void run() override;
};

#endif
