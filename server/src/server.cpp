#include "../includes/server.h"

#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <filesystem>
#include "server/includes/definitions.h"
#include "server/includes/gameloop.h"
#include "server/includes/acceptor.h"
#include "server/includes/monitor_queues.h"


Server::Server(const char* serverName): serverName(serverName), config(std::filesystem::path{CONFIG_PATH}) {
    // Deberia leer los archivos toml e inicializar las estructuras
}


void Server::start() {
    QueueCmd commands_queue;
    MonitorQueues monitor;

    Acceptor acceptorPlayers(this->serverName, monitor, commands_queue);
    acceptorPlayers.start();
    Gameloop gameWord(this->config, monitor, commands_queue);
    gameWord.start();

    std::string comandExit;
    while (std::getline(std::cin, comandExit)) {
        if (comandExit != "q") {
            continue;
        } else {
            break;
        }
    }
    acceptorPlayers.stop();
    gameWord.stop();

    acceptorPlayers.join();
    gameWord.join();
    acceptorPlayers.clear();
    commands_queue.close();
}

Server::~Server() {}
