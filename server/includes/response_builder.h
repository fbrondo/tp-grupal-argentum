#ifndef RESPONSE_BUILDER_H
#define RESPONSE_BUILDER_H
#include <map>
#include <memory>

#include "server/includes/core/snapshot.h"
#include "server/includes/player.h"

#include "world.h"

class ResponseBuilder {
private:
    /* data */
public:
    ResponseBuilder(/* args */) = default;
    Snapshot buildSnapshot(const std::map<Id, unique_ptr<Player>>& players, World& world);
    ~ResponseBuilder();
};

#endif
