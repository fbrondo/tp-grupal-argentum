// #include <iostream>

// #include "./common/includes/toml_config.h"

// int main(const int argc, char* argv[]) {
//     const char* config_path = (argc > 1) ? argv[1] : "config.toml";

//     // TODO: Delete this in the next iteration. Is a test.
//     try {
//         const TomlConfig config(config_path);

//         std::cout << "=== Argentum Server Config ===\n";
//         std::cout << "[server]" << std::endl;
//         std::cout << "  port:        " << config.get_or<int64_t>("server.port", 8080) << "\n";
//     } catch (const toml::parse_error& e) {
//         std::cerr << "Failed to read config: " << e << "\n";
//         return 1;
//     }

//     return 0;
// }

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
        Server sv(argv[SERVER_NAME]);
        sv.start();
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
