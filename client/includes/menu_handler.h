#pragma once

#include <cstdint>
#include <string>

#include "common/includes/socket.h"

#include "client_protocol.h"

class MenuHandler {
public:
    MenuHandler(const char* host, const char* port);

    bool doSignup(const std::string& user, const std::string& password, uint8_t race, uint8_t clase,
                  uint16_t head_id, uint16_t body_id);

    // Sends login and prints character info to stdout.
    bool doLogin(const std::string& user, const std::string& password);

private:
    Socket socket;
    ClientProtocol protocol;
};
