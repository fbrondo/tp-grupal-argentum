#pragma once
#include "common/includes/thread.h"
#include "server/includes/snapshot.h"
#include "client_protocol.h"

class ClientReceiver : public Thread {
    ClientProtocol& protocol;
    Queue<EventClient>& events_queue;

    void recvEventClient() const;

public:
    explicit ClientReceiver(ClientProtocol& protocol, Queue<EventClient>& events_queue);
    virtual void run() override;
};


