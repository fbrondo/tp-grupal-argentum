#ifndef RESPONSE_REGISTER_H
#define RESPONSE_REGISTER_H

#include <string>
#include "server/includes/responses/response.h"

class ResponseLogin: public Response {
private:
    bool success;
    std::string error_msg;

public:
    explicit ResponseLogin(bool success_);
    ResponseLogin(bool success_, const std::string& error_msg);
    ~ResponseLogin() override = default;

    void execute(ServerProtocol& protocol) override;
};

#endif
