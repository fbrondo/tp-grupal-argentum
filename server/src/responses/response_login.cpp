#include "../../includes/responses/response_login.h"


ResponseLogin::ResponseLogin(const bool success_, const std::string error_msg_ = ""): succes(success_), error_msg(error_msg_)  {
    
}

void ResponseLogin::execute(ServerProtocol& protocol){
    protocol.sendLoginResponse(this->succes, this->error_msg);
};