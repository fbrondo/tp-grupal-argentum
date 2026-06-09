#include "client/includes/menu_handler.h"

#include <iostream>

#include "common/includes/protocol.h"
#include "common/includes/core/character_traits.h"

MenuHandler::MenuHandler(const char* host, const char* port):
        socket(host, port), protocol(socket) {}

bool MenuHandler::doSignup(const std::string& user, const std::string& password,
                           const CharacterTraits& traits) {
    try {
        protocol.sendSignup(user, password, traits);
        CharacterTraits ch;
        protocol.sendSignup(user, password, ch );
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
//
// bool MenuHandler::doCreateCharacter(const std::string& user, const std::string& password,
//                                     const std::string& /*name*/, uint8_t /*race*/, uint8_t /*clase*/) {
//     try {
//         protocol.sendLogin(user, password);
//         std::string msg;
//         if (!protocol.recvResponse(LOGIN_RESPONSE, msg)) {
//             std::cerr << msg << std::endl;
//             return false;
//         }
//
//         //protocol.sendCharacterCreate(name, race, clase);
//         if (!protocol.recvResponse(CHARACTER_CREATE_RESPONSE, msg)) {
//             std::cerr << msg << std::endl;
//             return false;
//         }
//         return true;
//     } catch (const std::exception& e) {
//         std::cerr << e.what() << std::endl;
//         return false;
//     }
// }
