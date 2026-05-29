#ifndef RESPONSE_BUILDER_H
#define RESPONSE_BUILDER_H
#include <map>
#include "snapshot.h"
#include "player.h"
#include "world.h"

class ResponseBuilder {
private:
    /* data */
public:
    ResponseBuilder(/* args */) = default;
    Snapshot buildSnapshot(const std::map<Id,Player>& players, const World& world) {

    }
    ~ResponseBuilder();
};

#endif
