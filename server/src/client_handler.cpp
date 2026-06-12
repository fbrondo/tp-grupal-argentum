#include "server/includes/client_handler.h"


// Incluimos las nuevas abstracciones de este TP
// #include "command.h"

ClientHandler::ClientHandler(Id player_id, Socket&& socket, QueueCmd& cmd_q,
                             MonitorQueues& monitor):
        player_id(player_id),
        joinable(true),
        socket(std::move(socket)),
        protocol(this->socket),
        command_queue(cmd_q),
        monitor(monitor),
        send_queue(this->monitor.addQueuePlayer(this->player_id)),
        receiver(this->player_id, this->protocol, this->command_queue, this->send_queue),
        sender(this->protocol, this->send_queue) {}

void ClientHandler::start() {
    this->receiver.start();
    this->sender.start();
}

bool ClientHandler::is_alive() { return this->receiver.is_alive() || this->sender.is_alive(); }

void ClientHandler::stop() {
    this->receiver.stop();
    this->sender.stop();
}

void ClientHandler::join() {
    if (!this->joinable) {
        return;
    }
    this->joinable = false;
    this->receiver.join();
    this->sender.join();
}

ClientHandler::~ClientHandler() {
    // this->stop();
    // this->join();
    // this->monitor.removeQueuesPlayer(this->player_id);
}
