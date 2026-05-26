#ifndef ACCEPTOR_H
#define ACCEPTOR_H

#include <list>
#include <memory>

#include "common/includes/queue.h"
#include "common/includes/socket.h"
#include "common/includes/thread.h"
#include "common/includes/types.h"

#include "client_handler.h"
#include "definitions.h"
#include "monitor_queues.h"


// El hilo acceptor tiene la responsabilidad de aceptar clientes.
class Acceptor: public Thread {

private:
    Id next_id{0};
    Socket listen;
    MonitorQueues& monitor;
    QueueCmd& queue_cmd;
    std::list<std::unique_ptr<ClientHandler>> players;

    void reap();

public:
    explicit Acceptor(const char* serverName, MonitorQueues& monitor, QueueCmd& queue_cmd_);
    void run() override;
    void stop() override;
    void clear();
};

#endif
