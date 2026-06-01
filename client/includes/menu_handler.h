#pragma once

#include <string>

#include "common/includes/socket.h"

#include "client_protocol.h"

class MenuHandler {
public:
    MenuHandler(const char* host, const char* port);

    bool doSignup(const std::string& user, const std::string& password);

    // Sends login and prints character info to stdout.
    bool doLogin(const std::string& user, const std::string& password);

    bool doCreateCharacter(const std::string& user, const std::string& password,
                           const std::string& name, uint8_t race, uint8_t clase);

private:
    Socket socket;
    ClientProtocol protocol;
};
