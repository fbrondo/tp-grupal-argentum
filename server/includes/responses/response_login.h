#ifndef RESPONSE_REGISTER_H
#define RESPONSE_REGISTER_H

#include <string>
#include "response.h"
#include "../server_protocol.h"

class ResponseLogin : public Response{
private:
    bool succes;
    std::string error_msg;
public:
    ResponseLogin(const bool succes, const std::string error_msg = "");
    ~ResponseLogin() = default;
     
    void execute(ServerProtocol& protocol) override ;
};


#endif