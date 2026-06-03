#ifndef SERVER_H
#define SERVER_H

#include "server/includes/game_config_loader.h"
class Server {

private:
    const char* serverName;  // Pasar una referencia al acceptor
    GameConfigLoader config;

public:
    Server(const Server& other) = delete;
    Server& operator=(const Server& other) = delete;

    explicit Server(const char* servername, const char* config_path);
    void start();
    ~Server();
};

#endif
