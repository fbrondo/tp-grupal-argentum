#pragma once
#include <string>

#include "server/includes/responses/response.h"


class ResponseChatMsg: public Response {
private:
    std::string message;

public:
    explicit ResponseChatMsg(std::string message);
    void execute(ServerProtocol& protocol) override;
};
