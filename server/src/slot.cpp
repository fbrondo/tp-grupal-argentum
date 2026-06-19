#include "server/includes/slot.h"

Slot::Slot(const uint32_t& max_slot): quantity(0), max_slot(max_slot) {}

bool Slot::isFull() const { return quantity == max_slot; }

bool Slot::isEmpty() const { return instance == nullptr; }

void Slot::increase() { this->quantity += 1; }

void Slot::decrease() {
    if (isEmpty())
        return;
    if (1 > quantity) {
        quantity = 0;
        instance.reset();
    } else {
        quantity -= 1;
    }
}

void Slot::setItem(std::unique_ptr<ItemInstance>&& instance_) {
    this->instance = std::move(instance_);
}

ItemInstance* Slot::getItemInstance() { return instance.get(); }

uint32_t Slot::getQuantity() const {
    if (this->isEmpty()) {
        return 0;
    }
    return this->quantity;
}

TypeItem Slot::getTypeItem() const {
    if (this->isEmpty()) {
        return NONE;
    }
    auto inst = this->instance.get();
    return inst->item->type;
}

const Item* Slot::getItem() {
    auto inst = this->instance.get();
    return inst->item;
}
