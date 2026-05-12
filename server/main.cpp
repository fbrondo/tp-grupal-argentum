#include <iostream>
#include "common/toml_config.h"

int main(int argc, char* argv[]) {
    const char* config_path = (argc > 1) ? argv[1] : "config.toml";

    // TODO: Delete this in the next iteration. Is a test. 
    try {
        TomlConfig config(config_path);

        std::cout << "=== Argentum Server Config ===\n";
        std::cout << "[server]" << std::endl;
        std::cout << "  port:        " << config.get_or<int64_t>("server.port", 8080) << "\n";
    } catch (const toml::parse_error& e) {
        std::cerr << "Failed to read config: " << e << "\n";
        return 1;
    }

    return 0;
}
