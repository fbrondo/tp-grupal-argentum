#include "client/includes/command_processor.h"

#include <iostream>
#include <string>

#include <SDL2/SDL.h>

#include "client/includes/client.h"
#include "client/includes/menu_handler.h"

CommandProcessor::CommandProcessor(int argc, char* argv[]): argc_(argc), argv_(argv) {}

int CommandProcessor::run() const {
    if (argc_ < BASE_ARG) {
        std::cerr << "Usage: " << argv_[0] << " <host> <port> [--mode args...]" << std::endl;
        return 1;
    }

    if (argc_ == BASE_ARG)
        return launchGame();

    const std::string mode(argv_[3]);

    if (mode == "--signup")
        return processSignup() ? 0 : 1;
    if (mode == "--login")
        return processLogin() ? 0 : 1;
    if (mode == "--create-character")
        return processCreateCharacter() ? 0 : 1;

    std::cerr << "Unknown flag: " << mode << std::endl;
    return 1;
}

bool CommandProcessor::processSignup() const {
    if (argc_ != SIGNUP_ARG) {
        std::cerr << "Usage: " << argv_[0] << " <host> <port> --signup <user> <password>"
                  << std::endl;
        return false;
    }
    MenuHandler handler(argv_[1], argv_[2]);
    return handler.doSignup(argv_[4], argv_[5]);
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

bool CommandProcessor::processCreateCharacter() const {
    if (argc_ != CREATE_CHARACTER_ARG) {
        std::cerr << "Usage: " << argv_[0] << " <host> <port> --create-character"
                  << " <user> <password> <name> <race> <class>" << std::endl;
        return false;
    }
    MenuHandler handler(argv_[1], argv_[2]);
    return handler.doCreateCharacter(argv_[4], argv_[5], argv_[6],
                                     static_cast<uint8_t>(std::stoi(argv_[7])),
                                     static_cast<uint8_t>(std::stoi(argv_[8])));
}

int CommandProcessor::launchGame() const {
    SDL(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    // SDL2pp::SDLTTF sdl_ttf;
    Client client(argv_[1], argv_[2]);
    client.launch();
    return 0;
}
