#include "../includes/client_handler.h"


// Incluimos las nuevas abstracciones de este TP
//#include "command.h"

ClientHandler::ClientHandler(Id player_id, Socket&& socket, QueueCmd& cmd_q, MonitorQueues& monitor):
        player_id(player_id),
        joinable(true),
        socket(std::move(socket)),
        protocol(this->socket),
        command_queue(cmd_q),
        monitor(monitor),
        send_queue(monitor.addQueuePlayer(player_id)),
        receiver(player_id, protocol, command_queue, this->send_queue),
        sender(protocol, this->send_queue) {}

void ClientHandler::start() {
    this->receiver.start();
    this->sender.start();
}

bool ClientHandler::is_alive() { return this->receiver.is_alive() || this->sender.is_alive(); }

void ClientHandler::stop() {
    this->receiver.stop();
    this->sender.stop();

    send_queue.close();
    //  try {
    //      this->socket.shutdown(2);
    //      this->socket.close();
    //  } catch (...) {}
}

void ClientHandler::join() {
    if (!this->joinable) {
        return;
    }
    this->joinable = false;
    this->receiver.join();
    this->sender.join();
}

 /*QueueResp& ClientHandler::get_send_queue() { return this->send_queue; }*/

ClientHandler::~ClientHandler() {
    this->stop();
    this->join();
}
