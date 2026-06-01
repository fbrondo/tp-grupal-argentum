#include "server/includes/server.h"

#include <sstream>
#include <string>
#include <filesystem>
#include "server/includes/definitions.h"
#include "server/includes/gameloop.h"
#include "server/includes/acceptor.h"
#include "server/includes/monitor_queues.h"


Server::Server(const char* serverName, const char* config_path): serverName(serverName), config(Path{config_path}) {
}


void Server::start() {
    std::cout << "Server started" << std::endl;
    QueueCmd commands_queue;

    MonitorQueues monitor;
    Acceptor acceptorPlayers(this->serverName, monitor, commands_queue);
    acceptorPlayers.start();

    std::cerr << "Antes de construir Gameloop..." << std::endl;
    Gameloop gameWord(this->config, monitor, commands_queue);
    std::cerr << "Gameloop construido, antes de start..." << std::endl;
    gameWord.start();
    std::cerr << "Gameloop started OK" << std::endl;

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
