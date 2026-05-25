#include "server_client_handler.h"
#include <utility>
#include <memory>

// Incluimos las nuevas abstracciones de este TP
#include "command.h"
#include "snapshot.h"

ClientHandler::ClientHandler(uint32_t player_id, Socket&& socket, 
                             Queue<std::unique_ptr<Command>>& cmd_q) :
        player_id(player_id),
        socket(std::move(socket)),
        protocol(this->socket), 
        command_queue(cmd_q),
        send_queue(),
        receiver(player_id, protocol, command_queue, send_queue),
        sender(protocol, send_queue),
        joinable(true) {}

void ClientHandler::start() {
    receiver.start();
    sender.start();
}

bool ClientHandler::is_alive() { 
    return receiver.is_alive() || sender.is_alive(); 
}

void ClientHandler::stop() {
    receiver.stop();
    sender.stop();

    send_queue.close(); 

    try {
        socket.shutdown(2);
        socket.close();
    } catch (...) {}
}

void ClientHandler::join() {
    if (!joinable) {
        return;
    }
    joinable = false;
    receiver.join();
    sender.join();
}

Queue<Snapshot>& ClientHandler::get_send_queue() { 
    return send_queue; 
}

ClientHandler::~ClientHandler() {
    stop();
    join();
}
