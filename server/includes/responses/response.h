#ifndef RESPONSE_H
#define RESPONSE_H

class ServerProtocol;

class Response {
public:
    Response(/* args */) = default;
    virtual ~Response() = default;
    /*Cada comando va a definir su propio execute*/ 
    virtual void execute(ServerProtocol& protocol) = 0;
};


#endif
