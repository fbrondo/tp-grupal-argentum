#ifndef CLIENT_RECEIVER_H
#define CLIENT_RECEIVER_H
//#pragma once

#include <memory>
#include <stdexcept>

#include "../../common/includes/types.h"
#include "common/includes/protocol.h"
#include "common/includes/queue.h"
#include "common/includes/thread.h"

#include "definitions.h"
#include "server_protocol.h"
#include "snapshot.h"

class ClientReceiver: public Thread {
private:
    Id player_id;
    ServerProtocol& protocol;
    QueueCmd& commands_queue;
    Queue<Snapshot>& outgoing_queue;

public:
    ClientReceiver(Id player_id, ServerProtocol& protocol, QueueCmd& commands_queue,
                   Queue<Snapshot>& outgoing_queue);
    void run() override;
};

#endif
