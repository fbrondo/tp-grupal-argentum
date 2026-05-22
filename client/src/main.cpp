#include <exception>
#include <iostream>

#include "./client/includes/client.h"
#include "SDL2pp/SDL2pp.hh"

int main() try {
    // TODO: cuando haya servidor, recibir host/port/usuario por argumento:
    // const char* host = (argc > 1) ? argv[1] : "localhost";
    // const char* port = (argc > 2) ? argv[2] : "8080";
    Client client;
    client.run();
    return 0;
} catch (std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
}
