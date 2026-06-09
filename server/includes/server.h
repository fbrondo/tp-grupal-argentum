#ifndef SERVER_H
#define SERVER_H

#include "server/includes/game_config_loader.h"
#include "server/includes/acceptor.h"
#include "server/includes/core/config.h"
#include "server/includes/definitions.h"
#include "server/includes/monitor_queues.h"

class Server {

private:
    const char* serverName;  // Pasar una referencia al acceptor
    GameConfigLoader load_config;
    QueueCmd commands_queue;
    MonitorQueues monitor;
    Acceptor acceptor;
    GameConfig config;

public:
    Server(const Server& other) = delete;
    Server& operator=(const Server& other) = delete;

    explicit Server(const char* servername, const char* config_path);
    void start();
    ~Server();
};

#endif
