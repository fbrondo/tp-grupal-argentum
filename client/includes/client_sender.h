#pragma once
#include "common/includes/thread.h"
#include "client/includes/commands/command_client.h"

#include "client_protocol.h"
#include <memory>

class ClientSender : public Thread {
private:
    ClientProtocol& protocol;
    Queue<std::unique_ptr<ClientCommand>>& cmd_queue;

    void sendCmd() const;

public:
    explicit ClientSender(ClientProtocol& protocol, Queue<std::unique_ptr<ClientCommand>>& cmd_queue);
    virtual void run() override;
};
