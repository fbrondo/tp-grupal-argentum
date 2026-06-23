#ifndef CLIENT_SENDER_H
#define CLIENT_SENDER_H

#include <memory>
#include <stdexcept>

#include "common/includes/protocol.h"
#include "common/includes/queue.h"
#include "common/includes/socket.h"
#include "common/includes/thread.h"

#include "server_protocol.h"


class ClientSender: public Thread {
private:
    ServerProtocol& protocol;
    QueueResp& outgoing_queue;

public:
    ClientSender(ServerProtocol& protocol, QueueResp& outgoing_queue);
    void run() override;
};

#endif
