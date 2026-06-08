#include "server/includes/responses/response_login.h"

#include "server/includes/server_protocol.h"

ResponseLogin::ResponseLogin(const bool success_): success(success_) {}

ResponseLogin::ResponseLogin(const bool success_, const std::string& error_msg):
        success(success_), error_msg(error_msg) {}

void ResponseLogin::execute(ServerProtocol& protocol) {
    protocol.sendLoginResponse(this->success, this->error_msg);
}
