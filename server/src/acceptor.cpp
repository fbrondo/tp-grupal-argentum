#include "server/includes/acceptor.h"

#include <algorithm>
#include <utility>

#include <arpa/inet.h>

#include "common/includes/liberror.h"


Acceptor::Acceptor(const char* serverName, MonitorQueues& monitor, QueueCmd& queue_cmd_):
        listen(serverName), monitor(monitor), queue_cmd(queue_cmd_) {}

void Acceptor::clear() {
    for (auto& player: this->players) {
        player->stop();
        player->join();
    }
    this->players.clear();
}

void Acceptor::run() {

    try {
        while (should_keep_running()) {
            std::cout << "Entro en acceptor..." << std::endl;
            Socket peer = this->listen.accept();
            Id player_id = this->next_id++;
            auto client = std::make_unique<ClientHandler>(player_id, std::move(peer), this->queue_cmd, this->monitor);
            std::cout << "Antes de start..." << std::endl;
            client->start();
            std::cout << "Luego de start..." << std::endl;
            this->reap();
            players.push_back(std::move(client));
        }
    } catch (const LibError& e) {
        if (should_keep_running()) {
            std::cerr << "Error en Acceptor::run -- Socket acceptador cerrado --" << e.what()
                      << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error en Acceptor::run --" << e.what() << std::endl;
    }
}

void Acceptor::stop() {
    Thread::stop();
    this->listen.shutdown(2);
    this->listen.close();
}

void Acceptor::reap() {
    this->players.remove_if([](std::unique_ptr<ClientHandler> const& player) {
        if (!player->is_alive()) {
            player->stop();
            player->join();
            return true;
        }
        return false;
    });
}
