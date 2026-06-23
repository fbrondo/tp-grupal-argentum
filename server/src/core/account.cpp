#include "server/includes/core/account.h"


Account::Account(const uint32_t& golden_): golden(golden_) {}

std::vector<SlotData> Account::getSlotsData() const {
    std::vector<SlotData> slots_data;
    for (size_t i = 0; i < this->safe_box.size(); i++) {
        if (!safe_box[i].isEmpty()) {
            const auto type_item = static_cast<uint8_t>(this->safe_box[i].getTypeItem());
            SlotData data(type_item, i, safe_box[i].getQuantity());
            slots_data.push_back(data);
        }
    }
    return slots_data;
}

void Account::loadItemAccount(const Item* item, const uint32_t& quantity) {
    Slot slot(10);
    slot.setItem(std::make_unique<ItemInstance>(item));
    for (uint32_t i = 0; i < quantity; i++) {
        slot.increase();
    }
    this->safe_box.emplace_back(std::move(slot));
}
