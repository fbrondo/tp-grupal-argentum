#include <cerrno>
#include <iostream>
#include <stdexcept>

#include "server/includes/server.h"

#define ARGC 2
#define SERVER_NAME 1

int main(int argc, char const* argv[]) {

    if (argc != ARGC) {
        return EXIT_FAILURE;
    }

    try {
        Server sv(argv[SERVER_NAME], CONFIG_PATH);
        sv.start();
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
