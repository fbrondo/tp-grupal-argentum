#include "server/includes/client_sender.h"

#include <memory>
#include <utility>

#include "server/includes/responses/response.h"

ClientSender::ClientSender(ServerProtocol& protocol, QueueResp& outgoing_queue):
        protocol(protocol), outgoing_queue(outgoing_queue) {}

void ClientSender::run() {
    try {
        while (should_keep_running()) {
            std::unique_ptr<Response> response_server;
            try {
                response_server = std::move(outgoing_queue.pop());
            } catch (const ClosedQueue&) {
                break;
            }
            response_server->execute(this->protocol);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error en ClientSender: " << e.what() << std::endl;
    } catch (...) {}
}
