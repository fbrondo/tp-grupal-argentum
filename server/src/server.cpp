#include "../includes/server.h"

#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include "../includes/definitions.h"
#include "../includes/gameloop.h"
#include "../includes/monitor_queues.h"

#include "../includes/player.h"
#include "../includes/core/clase.h"
#include "../includes/core/race.h"
Server::Server(const char* serverName): serverName(serverName) {
    // Deberia leer los archivos toml e inicializar las estructuras
}


void Server::start() {
    QueueCmd commands_queue;
    MonitorQueues monitor;

    Acceptor acceptorPlayers(this->serverName, monitor, commands_queue);
    acceptorPlayers.start();

    const Race ch_race = Race{ELF, "Elfo", Statics{100, 10, 2, 1}, 100, 1, 2};
    const Clase ch_clase = Clase{WARRIOR, "Guerrero", Statics{100, 10, 2, 1}, 50, 9, 1};
    uint16_t golden = 0;
    uint8_t level = 1;
    Position pos(4, 4);
    Player p(ch_race, ch_clase, golden, 1, std::move(pos));;
    Gameloop gameWord(monitor, commands_queue, p);
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
