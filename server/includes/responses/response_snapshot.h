#ifndef RESPONSE_SNAPSHOT_H
#define RESPONSE_SNAPSHOT_H

#include "response.h"
#include "../server_protocol.h"

class ResponseSnapshot : public Response{
private:
    /* data */
public:
    ResponseSnapshot(/* args */) = default;
    ~ResponseSnapshot() = default;
    /*Cada comando va a definir su propio execute*/ 
    void execute(ServerProtocol& protocol) override {};
};


#endif
