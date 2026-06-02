#pragma once
#include "server/includes/responses/response.h"
#include "server/includes/server_protocol.h"
#include <string>

class ResponseActionError : public Response {
private:
    std::string message;
public:
    explicit ResponseActionError(std::string message);
    void execute(ServerProtocol& protocol) override;
};
