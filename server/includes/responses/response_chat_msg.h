#pragma once
#include "server/includes/responses/response.h"
#include "server/includes/server_protocol.h"
#include <string>

class ResponseChatMsg : public Response {
private:
    std::string message;
public:
    explicit ResponseChatMsg(std::string message);
    void execute(ServerProtocol& protocol) override;
};
