#pragma once
#include <memory>

#include "client/includes/client_protocol.h"
#include "client/includes/commands/command_client.h"
#include "common/includes/queue.h"
#include "common/includes/thread.h"

class ClientSender: public Thread {
private:
    ClientProtocol& protocol;
    Queue<std::unique_ptr<ClientCommand>>& cmd_queue;

    void sendCmd() const;

public:
    explicit ClientSender(ClientProtocol& protocol,
                          Queue<std::unique_ptr<ClientCommand>>& cmd_queue);
    virtual void run() override;
};
