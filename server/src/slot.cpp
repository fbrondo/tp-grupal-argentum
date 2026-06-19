#include "server/includes/slot.h"

Slot::Slot(const uint32_t& max_slot): quantity(0), max_slot(max_slot) {}

bool Slot::isFull() const { return quantity == max_slot; }

bool Slot::isEmpty() const { return instance == nullptr; }

void Slot::increase() { this->quantity += 1; }

void Slot::decrease() {
    if (isEmpty())
        return;
    if (quantity <= 1) {
        quantity = 0;
        instance.reset();
    } else {
        quantity -= 1;
    }
}

void Slot::setItem(std::unique_ptr<ItemInstance>&& instance_) {
    this->instance = std::move(instance_);
}

std::unique_ptr<ItemInstance> Slot::takeOneItem() {
    if (isEmpty()) {
        return nullptr;
    }
    if (quantity > 1) {
        quantity -= 1;
        return std::make_unique<ItemInstance>(instance->item);
    }

    quantity = 0;
    return std::move(instance);
}

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
