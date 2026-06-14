#pragma once

#include <cstdint>
#include <memory>

#include "common/includes/types.h"
// #include "server/includes/gameloop.h"
class Gameloop;

class Command {
protected:
    Id client_id;

public:
    explicit Command(Id id_): client_id(id_) {}
    virtual ~Command() = default;

    const Id& getIdPlayer() { return this->client_id; }
    // Cada comando va a definir su propio execute
    virtual void execute(Gameloop& game) = 0;
};
