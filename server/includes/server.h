#ifndef SERVER_H
#define SERVER_H

#include "acceptor.h"

class Server {

private:
    const char* serverName;  // Pasar una referencia al acceptor

public:
    Server(const Server& other) = delete;
    Server& operator=(const Server& other) = delete;

    explicit Server(const char* serverName);
    void start();
    ~Server();
};

#endif
