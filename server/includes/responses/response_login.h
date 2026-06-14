#ifndef RESPONSE_REGISTER_H
#define RESPONSE_REGISTER_H

#include <string>

#include "common/includes/types.h"
#include "server/includes/responses/response.h"

class ResponseLogin: public Response {
private:
    bool success;
    Id player_id;
    std::string error_msg;

public:
    ResponseLogin(bool success_, Id player_id_, const std::string& error_msg = "");
    ~ResponseLogin() override = default;

    void execute(ServerProtocol& protocol) override;
};

#endif
