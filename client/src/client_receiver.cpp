#include "client/includes/client_receiver.h"
#include "common/includes/liberror.h"

ClientReceiver::ClientReceiver(ClientProtocol& protocol, Queue<EventClient>& events_queue):
        protocol(protocol), events_queue(events_queue) {}

void ClientReceiver::recvEventClient() const {
    EventClient event;
    if (!protocol.receiveMessage(event)) {
        events_queue.push(event);  // el evento ya tiene type = DISCONNECTION
        throw LibError(EPIPE, "server closed connection");
    }
    events_queue.push(event);
}

void ClientReceiver::run() {
    try {
        while (should_keep_running()) {
            recvEventClient();
        }
    } catch (const ClosedQueue&) {
        // No imprimimos error aquí porque es la forma normal de terminar el hilo
    } catch (const LibError& e) {
        /// El socket se cerró o falló
    } catch (const std::exception& e) {
        std::cerr << "[ERROR:Receiver] Excepción inesperada en hilo Receiver: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "[ERROR:Receiver] Error desconocido." << std::endl;
    }
}