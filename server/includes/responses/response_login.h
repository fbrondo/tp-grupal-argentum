#ifndef RESPONSE_REGISTER_H
#define RESPONSE_REGISTER_H

#include <string>
#include "response.h"
#include "../server_protocol.h"

class ResponseLogin : public Response{
private:
    bool success;
    std::string error_msg;
public:
    explicit ResponseLogin(bool success_);
    ResponseLogin(bool success_, std::string error_msg);
    ~ResponseLogin() override = default;
     
    void execute(ServerProtocol& protocol) override ;
};


#endif