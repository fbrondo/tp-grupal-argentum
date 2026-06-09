#include "server/includes/server.h"

#include <filesystem>
#include <sstream>
#include <string>

#include "server/includes/acceptor.h"

#include "server/includes/gameloop.h"
#include "server/print.h"

Server::Server(const char* serverName, const char* config_path):
        serverName(serverName),load_config(Path{config_path}),
        acceptor(serverName,this->monitor,this->commands_queue) {}


void Server::start() {
    Print::initServer();
    //QueueCmd commands_queue;
    //MonitorQueues monitor;
    //Acceptor acceptorPlayers(this->serverName, monitor, commands_queue);
    acceptor.start();
    GameConfig conf = this->load_config.getdGameConfiguration();
    Gameloop gameWord(std::move(conf), monitor, commands_queue);
    gameWord.start();

    std::string comand_exit;
    while (std::getline(std::cin, comand_exit)) {
        if (comand_exit != "q") {
            continue;
        } else {
            break;
        }
    }
    this->acceptor.stop();
    gameWord.stop();

    this->acceptor.join();
    gameWord.join();

    this->acceptor.clear();
}

Server::~Server() {
    this->commands_queue.close();
}
