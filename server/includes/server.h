#ifndef SERVER_H
#define SERVER_H

#include "server/includes/game_config.h"
class Server {

private:
    const char* serverName;  // Pasar una referencia al acceptor
    GameConfig config;

public:
    Server(const Server& other) = delete;
    Server& operator=(const Server& other) = delete;

    explicit Server(const char* serverName);
    void start();
    ~Server();
};

#endif
