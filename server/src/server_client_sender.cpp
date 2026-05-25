#include "server_client_sender.h"

ClientSender::ClientSender(ServerProtocol& protocol, 
                           Queue<Snapshot>& outgoing_queue) :
        protocol(protocol),
        outgoing_queue(outgoing_queue) {}

void ClientSender::run() {
    try {
        while (should_keep_running()) {
            Snapshot snapshot;
            
            try {
                snapshot = outgoing_queue.pop();
            } catch (const ClosedQueue&) {
                break;
            }

            protocol.sendSnapshot(snapshot);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error en ClientSender: " << e.what() << std::endl;
    } catch (...) {}

    protocol.shutdown_peer();
}
