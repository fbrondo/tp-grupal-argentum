#include "server/includes/slot.h"

Slot::Slot(const uint32_t &max_slot): quantity(0), max_slot(max_slot) {}

bool Slot::isFull() const { return quantity == max_slot;}

bool Slot::isEmpty() const { return instance == nullptr; }

void Slot::increase() { this->quantity+=1; }

void Slot::decrease() {
    if (isEmpty()) return;
    if ( 1 > quantity) {
        quantity = 0;
        instance.reset();
    } else {
        quantity -= 1;
    }
}

void Slot::setItem(std::unique_ptr<ItemInstance> &&instance_) {
    this->instance = std::move(instance_);
}

uint32_t Slot::getQuantity() const {return this->quantity;}
}

TypeItem Slot::getTypeItem() const {
    auto inst = this->instance.get();
    return inst->item->type;
}

const Item * Slot::getItem() {
    auto inst = this->instance.get();
    return inst->item;
}
