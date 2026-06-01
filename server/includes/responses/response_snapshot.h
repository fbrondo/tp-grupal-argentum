#ifndef RESPONSE_SNAPSHOT_H
#define RESPONSE_SNAPSHOT_H

#include "response.h"
#include "server/includes/server_protocol.h"
#include "server/includes/core/snapshot.h"

class ResponseSnapshot : public Response {
private:
    Snapshot snap;
public:
    explicit ResponseSnapshot(Snapshot&& snp);
    ~ResponseSnapshot() override = default;
    /*Cada comando va a definir su propio execute*/ 
    void execute(ServerProtocol& protocol) override;
};


#endif
