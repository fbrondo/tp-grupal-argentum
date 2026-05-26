#ifndef CLIENT_SENDER_H
#define CLIENT_SENDER_H
//#pragma once

#include <memory>
#include <stdexcept>

#include "common/includes/protocol.h"
#include "common/includes/queue.h"
#include "common/includes/socket.h"
#include "common/includes/thread.h"

#include "server_protocol.h"
#include "snapshot.h"


class ClientSender: public Thread {
private:
    ServerProtocol& protocol;
    Queue<Snapshot>& outgoing_queue;

public:
    ClientSender(ServerProtocol& protocol, Queue<Snapshot>& outgoing_queue);
    void run() override;
};

#endif
