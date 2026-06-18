#ifndef SLOT_H
#define SLOT_H

#include <cstdint>
#include <memory>

#include "common/includes/types.h"
#include "server/includes/core/data.h"
#include "server/includes/core/instances.h"

class Slot {
private:
    std::unique_ptr<ItemInstance> instance;  // "Slot Vacío"
    uint32_t quantity;                       // Cuántos ítems de este tipo hay apilados;
    uint32_t max_slot;

public:
    Slot(const Slot& other) = delete;
    Slot& operator=(const Slot& other) = delete;

    Slot(): quantity(0), max_slot(0) {}
    Slot(Slot&&) = default;
    Slot& operator=(Slot&&) = default;

    explicit Slot(const uint32_t& max_slot);
    [[nodiscard]] bool isFull() const;
    [[nodiscard]] bool isEmpty() const;
    void increase();
    void decrease();
    void setItem(std::unique_ptr<ItemInstance>&& instance_);

    uint32_t getQuantity() const;
    TypeItem getTypeItem() const;
    const Item* getItem();
};


#endif  // TALLER_TP_SLOT_H
