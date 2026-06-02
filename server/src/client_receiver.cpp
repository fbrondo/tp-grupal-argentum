#include "server/includes/client_receiver.h"

#include <cstdint>
#include <memory>
#include <stdexcept>

#include "common/includes/queue.h"

ClientReceiver::ClientReceiver(const Id& player_id, ServerProtocol& protocol,
                               QueueCmd& commands_queue, QueueResp& outgoing_queue):
        player_id(player_id),
        protocol(protocol),
        commands_queue(commands_queue),
        outgoing_queue(outgoing_queue) {}

void ClientReceiver::run() {
    try {
        while (should_keep_running()) {
            bool connected = protocol.readCommand(this->player_id, this->commands_queue);
            if (!connected) {
                break;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error en ClientReceiver (" << this->player_id << "): " << e.what()
                  << std::endl;
    } catch (...) {}
    /*Avisamos al GameLoop que este jugador ya no está para que limpie el modelo*/
    try {
        commands_queue.push(std::make_unique<DisconnectCommand>(this->player_id));
    } catch (const ClosedQueue&) {}

    // try {
    //     outgoing_queue.close();
    // } catch (...) {}
}

ClientReceiver::~ClientReceiver() {
    try {
        this->outgoing_queue.close();
    } catch (...) {}
}
