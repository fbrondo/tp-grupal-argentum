#pragma once

#include <cstdint>
#include <memory>

// Lo delcaro por adelantado asumiendo que le pasamos el mundo para ejecutar un comando
class World;

class Command {
protected:
    uint32_t client_id;

public:
    explicit Command(uint32_t id): client_id(id) {}
    virtual ~Command() = default;

    // Cada comando va a definir su propio execute
    virtual void execute(World& world) = 0;
};
