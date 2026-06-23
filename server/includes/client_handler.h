#ifndef CLIENTHANDLER_H
#define CLIENTHANDLER_H

#include <memory>
#include <string>

#include "common/includes/protocol.h"
#include "common/includes/queue.h"
#include "common/includes/types.h"

#include "client_receiver.h"
#include "client_sender.h"
#include "definitions.h"
#include "monitor_queues.h"

/*Cada vez que un cliente se conecta, el servidor crea un ClientHandler para encapsular toda la
 lógica relacionada a esa conexión. Se encarga de mantener el estado del cliente y coordinar el
 envío y recepción de mensajes usando threads separados. */

class ClientHandler {
private:
    Id player_id;
    bool joinable;
    Socket socket;
    ServerProtocol protocol;

    QueueCmd& command_queue;
    MonitorQueues& monitor;
    QueueResp& send_queue;

    ClientReceiver receiver;
    ClientSender sender;

public:
    ClientHandler(const ClientHandler&) = delete;
    ClientHandler& operator=(const ClientHandler&) = delete;

    ClientHandler(Id id_, Socket&& socket, QueueCmd& cmd_q, MonitorQueues& monitor);

    void start();
    void stop();
    void join();

    bool is_alive();
    ~ClientHandler();
};

#endif
