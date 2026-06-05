#include "client/includes/command_processor.h"

#include <iostream>
#include <string>

#include <SDL2/SDL.h>

#include "client/includes/client.h"
#include "client/includes/menu_handler.h"
#include "common/includes/core/character_traits.h"

CommandProcessor::CommandProcessor(int argc, char* argv[]): argc_(argc), argv_(argv) {}

int CommandProcessor::run() const {
    if (argc_ < MIN_ARGS) {
        std::cerr << "For help: " << argv_[0] << " --help" << std::endl;
        return 1;
    }

    if (argc_ == BASE_ARG)
        return launchGame();

    if (argc_ == MIN_ARGS) {
        if (std::string(argv_[1]) == "--help" || std::string(argv_[1]) == "-h") {
            printHelp();
            return 0;
        }
        std::cerr << "For help: " << argv_[0] << " --help" << std::endl;
        return 1;
    }

    const std::string mode(argv_[3]);

    if (mode == "--signup")
        return processSignup() ? 0 : 1;
    if (mode == "--login")
        return processLogin() ? 0 : 1;
    if (mode == "--help" || mode == "-h") {
        printHelp();
        return 0;
    }

    std::cerr << "Unknown flag: " << mode << ". Run with --help for usage." << std::endl;
    return 1;
}

bool CommandProcessor::processSignup() const {
    if (argc_ != SIGNUP_ARG) {
        std::cerr << "Usage: " << argv_[0]
                  << " <host> <port> --signup <user> <password> <race> <class> <head> <body>"
                  << std::endl;
        return false;
    }
    const CharacterTraits traits{
            static_cast<uint16_t>(std::stoi(argv_[8])),  // head
            static_cast<uint16_t>(std::stoi(argv_[9])),  // body
            static_cast<uint8_t>(std::stoi(argv_[6])),   // race
            static_cast<uint8_t>(std::stoi(argv_[7]))    // clase
    };
    MenuHandler handler(argv_[1], argv_[2]);
    return handler.doSignup(argv_[4], argv_[5], traits);
}

bool CommandProcessor::processLogin() const {
    if (argc_ != LOGIN_ARG) {
        std::cerr << "Usage: " << argv_[0] << " <host> <port> --login <user> <password>"
                  << std::endl;
        return false;
    }
    MenuHandler handler(argv_[1], argv_[2]);
    return handler.doLogin(argv_[4], argv_[5]);
}

void CommandProcessor::printHelp() const {
    std::cout << "Usage: " << argv_[0] << " <host> <port> [--mode args...]\n"
              << "\n"
              << "Modes:\n"
              << "  (no flag)       Launch game client (SDL)\n"
              << "  --login   <user> <password>\n"
              << "                  Login and print character info to stdout\n"
              << "  --signup  <user> <password> <race> <class> <head> <body>\n"
              << "                  Register a new account with character\n"
              << "                  race:  0=Human 1=Elf 2=Dwarf 3=Gnome  (0-based)\n"
              << "                  class: 0=Wizard 1=Cleric 2=Paladin 3=Warrior\n"
              << "                  head/body: sprite id (1-" << MAX_HEAD_ID << ")\n"
              << "  --help, -h      Show this help\n"
              << "\n"
              << "Examples:\n"
              << "  " << argv_[0] << " localhost 8080\n"
              << "  " << argv_[0] << " localhost 8080 --login myuser mypass\n"
              << "  " << argv_[0] << " localhost 8080 --signup myuser mypass 0 0 1 1\n";
}

int CommandProcessor::launchGame() const {
    SDL(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    // SDL2pp::SDLTTF sdl_ttf;
    Client client(argv_[1], argv_[2]);
    client.launch();
    return 0;
}
