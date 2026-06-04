#include "client/includes/menu_handler.h"

#include <iostream>

#include "common/includes/protocol.h"

MenuHandler::MenuHandler(const char* host, const char* port):
        socket(host, port), protocol(socket) {}

bool MenuHandler::doSignup(const std::string& user, const std::string& password,
                           const CharacterTraits& traits) {
    try {
        protocol.sendSignup(user, password, traits);
        std::string msg;
        if (!protocol.recvResponse(SIGNUP_RESPONSE, msg)) {
            std::cerr << msg << std::endl;
            return false;
        }
        return true;
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return false;
    }
}

bool MenuHandler::doLogin(const std::string& user, const std::string& password) {
    try {
        protocol.sendLogin(user, password);
        std::string payload;
        if (!protocol.recvResponse(LOGIN_RESPONSE, payload)) {
            std::cerr << payload << std::endl;
            return false;
        }
        std::cout << payload << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return false;
    }
}
