#include "client/includes/command_processor.h"

#include <iostream>
#include <string>

#include <SDL2/SDL.h>

#include "client/includes/client.h"
#include "client/includes/core/constants.h"
#include "client/includes/menu_handler.h"
#include "common/includes/core/character_traits.h"

CommandProcessor::CommandProcessor(int argc, char* argv[]): argc_(argc), argv_(argv) {}

int CommandProcessor::run() const {
    if (argc_ < MIN_ARGS) {
        std::cerr << "For help: " << argv_[0] << " --help" << std::endl;
        return 1;
    }

    if (argc_ == BASE_ARG)
        return launchGame("", "");

    if (argc_ == GAME_ARG)
        return launchGame(argv_[3], argv_[4]);

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
        return processSignup();
    if (mode == "--login")
        return processLogin();
    if (mode == "--help" || mode == "-h") {
        printHelp();
        return 0;
    }

    std::cerr << "Unknown flag: " << mode << ". Run with --help for usage." << std::endl;
    return 1;
}

int CommandProcessor::processSignup() const {
    if (argc_ != SIGNUP_ARG) {
        std::cerr << "Usage: " << argv_[0]
                  << " <host> <port> --signup <user> <password> <race> <class> <head> <body>"
                  << std::endl;
        return 1;
    }
    const CharacterTraits traits{
            static_cast<uint16_t>(std::stoi(argv_[8])),  // head
            static_cast<uint16_t>(std::stoi(argv_[9])),  // body
            static_cast<uint8_t>(std::stoi(argv_[6])),   // race
            static_cast<uint8_t>(std::stoi(argv_[7]))    // clase
    };
    MenuHandler handler(argv_[1], argv_[2]);
    if (!handler.doSignup(argv_[4], argv_[5], traits))
        return 1;
    return 0;
}

int CommandProcessor::processLogin() const {
    if (argc_ != LOGIN_ARG) {
        std::cerr << "Usage: " << argv_[0] << " <host> <port> --login <user> <password>"
                  << std::endl;
        return 1;
    }
    MenuHandler handler(argv_[1], argv_[2]);
    if (!handler.doLogin(argv_[4], argv_[5]))
        return 1;
    return 0;
}

void CommandProcessor::printHelp() const {
    std::cout << "Usage: " << argv_[0] << " <host> <port> [options]\n"
              << "\n"
              << "Modes:\n"
              << "  (no flag)       Launch game (prompts login)\n"
              << "  <user> <pass>   Login and launch game\n"
              << "  --signup  <user> <password> <race> <class> <head> <body>\n"
              << "                  Register a new account and launch game\n"
              << "                  race:  1=Human 2=Elf 3=Dwarf 4=Gnome\n"
              << "                  class: 1=Wizard 2=Cleric 3=Paladin 4=Warrior\n"
              << "                  head/body: sprite id (1-" << MAX_HEAD_ID << ")\n"
              << "  --help, -h      Show this help\n"
              << "\n"
              << "Examples:\n"
              << "  " << argv_[0] << " localhost 8080 myuser mypass\n"
              << "  " << argv_[0] << " localhost 8080 --signup myuser mypass 1 1 1 1\n";
}

int CommandProcessor::launchGame(const std::string& user, const std::string& pass) const {
    SDL(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    Client client(argv_[1], argv_[2]);
    client.launch(user, pass);
    return 0;
}
