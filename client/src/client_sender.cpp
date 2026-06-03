#include "client/includes/client_sender.h"

#include "client/includes/commands/command_move.h"
#include "common/includes/liberror.h"

ClientSender::ClientSender(ClientProtocol& protocol,
                           Queue<std::unique_ptr<ClientCommand>>& cmd_queue):
        protocol(protocol), cmd_queue(cmd_queue) {}

void ClientSender::sendCmd() const {
    const std::unique_ptr<ClientCommand> cmd = cmd_queue.pop();
    cmd->execute(protocol);
}

void ClientSender::run() {
    try {
        while (should_keep_running()) {
            sendCmd();
        }
    } catch (const ClosedQueue& e) {
        // No imprimimos error acá porque es la forma normal de terminar el hilo
    } catch (const LibError& e) {
        std::cerr << "[ERROR:Sender] Error de red al enviar mensaje: " << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "[ERROR:Sender] Excepción inesperada: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "[ERROR:Sender] Error desconocido." << std::endl;
    }
}
