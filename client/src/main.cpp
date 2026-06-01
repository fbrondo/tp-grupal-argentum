#include <exception>
#include <iostream>

#include "./client/includes/command_processor.h"
#include "SDL2pp/SDL2pp.hh"

int main(const int argc, char* argv[]) {
    try {
        return CommandProcessor(argc, argv).run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
