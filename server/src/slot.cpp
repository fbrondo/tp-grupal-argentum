#include "server/includes/slot.h"

Slot::Slot(const uint32_t& capacity_slot): quantity(0), capacity_slot(capacity_slot) {}

bool Slot::isFull() const { return this->quantity == this->capacity_slot; }

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
    if (this->isEmpty()) {
        return nullptr;
    }
    auto inst = std::make_unique<ItemInstance>(this->instance->item);
    this->decrease();
    return inst;
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
