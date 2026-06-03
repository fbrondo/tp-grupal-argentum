#pragma once
#include "client/includes/client_protocol.h"
#include "common/includes/thread.h"
#include "server/includes/core/snapshot.h"

class ClientReceiver: public Thread {
    ClientProtocol& protocol;
    Queue<EventClient>& events_queue;

    void recvEventClient() const;

public:
    explicit ClientReceiver(ClientProtocol& protocol, Queue<EventClient>& events_queue);
    virtual void run() override;
};
