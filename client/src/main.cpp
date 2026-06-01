#include <exception>
#include <iostream>

#include "./client/includes/client.h"
#include "SDL2pp/SDL2pp.hh"

#define NUMBER_OF_ARGUMENTS_CLIENT 3

int main(const int argc, char* argv[]) {
    if (argc != NUMBER_OF_ARGUMENTS_CLIENT) {
        std::cerr << "Mala llamada al programa. Esperado: " << argv[0]
                  << " <hostname o IP> <servicename o puerto>" << std::endl;
        return 1;
    }
    try {
        const char* hostname = argv[1];
        const char* servname = argv[2];
        Client client(hostname, servname);
        client.launch();
        return 0;
    } catch (std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
