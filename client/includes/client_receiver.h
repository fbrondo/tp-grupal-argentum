#pragma once
#include "../../common/includes/core/snapshot.h"
#include "client/includes/client_protocol.h"
#include "common/includes/thread.h"

class ClientReceiver: public Thread {
    ClientProtocol& protocol;
    Queue<EventClient>& events_queue;

    void recvEventClient() const;

public:
    explicit ClientReceiver(ClientProtocol& protocol, Queue<EventClient>& events_queue);
    virtual void run() override;
};
