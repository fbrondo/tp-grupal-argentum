#pragma once

#include <string>

#include "server/includes/responses/response.h"
#include "server/includes/server_protocol.h"

class ResponseSignup: public Response {
private:
    bool success;
    std::string message;

public:
    explicit ResponseSignup(bool success_);
    ResponseSignup(bool success_, const std::string& message_);
    ~ResponseSignup() override = default;
    void execute(ServerProtocol& protocol) override;
};
