#pragma once

#include <string>

#include "common/includes/core/character_traits.h"
#include "common/includes/socket.h"

#include "client_protocol.h"

class MenuHandler {
public:
    MenuHandler(const char* host, const char* port);

    bool doSignup(const std::string& user, const std::string& password,
                  const CharacterTraits& traits);

    // Sends login and prints character info to stdout.
    bool doLogin(const std::string& user, const std::string& password);

private:
    Socket socket;
    ClientProtocol protocol;
};
