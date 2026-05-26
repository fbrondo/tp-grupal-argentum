#include "../includes/server.h"

#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

#include "../includes/gameloop.h"
#include "../includes/monitor_queues.h"

Server::Server(const char* serverName): serverName(serverName) {
    // Deberia leer los archivos toml e inicializar las estructuras
}


void Server::start() {
    RequestQueue requestsPlayer;
    MonitorQueues monitor;

    Acceptor acceptorPlayers(this->serverName, monitor, requestsPlayer);
    acceptorPlayers.start();

    Gameloop gameWord(this->NPCs, monitor, requestsPlayer);
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
    requestsPlayer.close();
}

Server::~Server() {}
