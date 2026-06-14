#include "server/includes/responses/response_login.h"

#include "server/includes/server_protocol.h"

ResponseLogin::ResponseLogin(const bool success_, Id player_id_, const std::string& error_msg):
        success(success_), player_id(player_id_), error_msg(error_msg) {}

void ResponseLogin::execute(ServerProtocol& protocol) {
    protocol.sendLoginResponse(this->success, this->player_id, this->error_msg);
}
