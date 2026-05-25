#pragma once

#include <string>

#include "common/includes/protocol.h"
#include "common/includes/queue.h"
#include "server_client_receiver.h"
#include "server_client_sender.h"

/*Cada vez que un cliente se conecta, el servidor crea un ClientHandler para encapsular toda la
 lógica relacionada a esa conexión. Se encarga de mantener el estado del cliente y coordinar el
 envío y recepción de mensajes usando threads separados. */

class ClientHandler {
private:
    bool joinable = true;
    Socket socket;
    ServerProtocol protocol;

    uint32_t player_id;

    Queue<std::unique_ptr<Command>>& command_queue;
    Queue<Snapshot> send_queue;

    ClientReceiver receiver;
    ClientSender sender;

public:
    ClientHandler(uint32_t player_id, Socket&& socket, Queue<std::unique_ptr<Command>>& cmd_q);

    void start();
    void stop();
    void join();

    Queue<Snapshot>& get_send_queue();

    bool is_alive();

    ~ClientHandler();

    ClientHandler(const ClientHandler&) = delete;
    ClientHandler& operator=(const ClientHandler&) = delete;
};
