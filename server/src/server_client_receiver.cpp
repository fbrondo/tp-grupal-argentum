#include "../includes/server_client_receiver.h"

ClientReceiver::ClientReceiver(uint32_t player_id, ServerProtocol& protocol,
                               Queue<std::unique_ptr<Command>>& commands_queue,
                               Queue<Snapshot>& outgoing_queue):
        player_id(player_id),
        protocol(protocol),
        commands_queue(commands_queue),
        outgoing_queue(outgoing_queue) {}

void ClientReceiver::run() {
    try {

        while (should_keep_running()) {

            bool socket_alive = protocol.readCommand(player_id, commands_queue);

            if (!socket_alive) {
                break;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error en ClientReceiver (" << player_id << "): " << e.what() << std::endl;
    } catch (...) {}

    // Avisamos al GameLoop que este jugador ya no está para que limpie el modelo
    try {
        commands_queue.push(std::make_unique<DisconnectCommand>(player_id));
    } catch (const ClosedQueue&) {}

    try {
        outgoing_queue.close();
    } catch (...) {}

    protocol.shutdown_peer();
}
