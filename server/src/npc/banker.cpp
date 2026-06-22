#include "server/includes/npc/banker.h"

Banker::Banker(TypeNPC type, const std::string& name, Bank& bank_, const Pose& pose_):
        CitizenNPC(type, name, pose_), bank(bank_) {}

void Banker::incrementSlotSafeBox(const std::string& username, TypeItem type_item) {
    std::vector<Slot>& slots = this->bank.accounts[username].safe_box;
    for (auto& slot: slots) {
        if (!slot.isEmpty() && slot.getTypeItem() == type_item) {
            slot.increase();
            break;
        }
    }
}

void Banker::increaseGoldAccount(const std::string& username, const uint32_t& amount) {
    auto& gold_current_dep = this->bank.accounts[username].golden;
    gold_current_dep += amount;
}

uint32_t Banker::decreaseGoldAccount(const std::string& username, const uint32_t& amount) {
    auto& gold_current_dep = this->bank.accounts[username].golden;
    const uint32_t withdrawn = std::min(amount, gold_current_dep);
    gold_current_dep -= withdrawn;
    return withdrawn;
}

uint32_t Banker::getGoldDepositedPlayer(const std::string& username) const {
    return this->bank.accounts[username].golden;
}

void Banker::setItemSafeBox(const std::string& username, const Item* item) {
    std::vector<Slot>& slots = this->bank.accounts[username].safe_box;
    Slot slot(10);  // capacity
    slot.setItem(std::make_unique<ItemInstance>(item));
    slot.increase();
    slots.emplace_back(std::move(slot));
    // for (auto& slot: slots) {
    //     if (slot.isEmpty()) {
    //         slot.setItem(std::make_unique<ItemInstance>(item));
    //         slot.increase();
    //         break;
    //     }
    // }
}

void Banker::createPlayerAccount(const std::string& username) {
    this->bank.accounts.emplace(username, Account());
}

std::optional<size_t> Banker::hasItemInAccountPlayer(const std::string& username, TypeItem type) {
    std::vector<Slot>& slots = this->bank.accounts[username].safe_box;
    for (size_t i = 0; i < slots.size(); i++) {
        if (!slots[i].isEmpty() && slots[i].getTypeItem() == type) {
            return i;
        }
    }
    return std::nullopt;
}

bool Banker::thePlayerHasAnAccount(const std::string& username) {
    return this->bank.accounts.contains(username);
}

bool Banker::playerWithdrawItem(Player& player, TypeItem type) {
    const auto username = player.getName();
    if (!this->thePlayerHasAnAccount(username)) {
        this->createPlayerAccount(username);
        return false;
    }
    std::optional<size_t> index = this->hasItemInAccountPlayer(username, type);
    if (!index.has_value()) {
        return false;
    }
    auto& slots = this->bank.accounts[username].safe_box;
    auto& slot = slots[index.value()];  // cppcheck-suppress syntaxError
    const ItemInstance item(slot.getItem());
    player.addItemToInventory(item);
    slot.decrease();
    return true;
}

bool Banker::playerDepositItem(Player& player, TypeItem type) {
    const auto username = player.getName();
    if (!this->thePlayerHasAnAccount(username)) {
        this->createPlayerAccount(username);
    }
    const auto item = player.removeItemFromInventory(type);
    if (!item) {
        return false;
    }
    std::optional<size_t> index = this->hasItemInAccountPlayer(username, item->type);
    if (index.has_value()) {
        this->incrementSlotSafeBox(username, item->type);
    } else {
        this->setItemSafeBox(username, item);
    }
    return true;
}

uint32_t Banker::playerDepositGold(Player& player, const uint32_t& amount) {
    const auto username = player.getName();
    if (!this->thePlayerHasAnAccount(username)) {
        this->createPlayerAccount(username);
    }
    const auto gold_dep = player.decreaseGold(amount);
    this->increaseGoldAccount(username, gold_dep);
    return gold_dep;
}

uint32_t Banker::playerWithdrawGold(Player& player, const uint32_t& amount) {
    const auto username = player.getName();
    if (!this->thePlayerHasAnAccount(username)) {
        this->createPlayerAccount(username);
        return 0;
    }
    const auto gold_withdraw = this->decreaseGoldAccount(username, amount);
    player.increaseGold(gold_withdraw);
    return gold_withdraw;
}

std::map<TypeItem, uint32_t> Banker::depositedItems(Player& player) {
    std::map<TypeItem, uint32_t> items;
    const auto username = player.getName();
    if (!this->thePlayerHasAnAccount(username)) {
        this->createPlayerAccount(username);
        return items;
    }
    const auto& slots = this->bank.accounts[username].safe_box;
    for (const auto& slot: slots) {
        if (!slot.isEmpty()) {
            const auto type_iem = slot.getTypeItem();
            const auto quantity = slot.getQuantity();
            items.emplace(type_iem, quantity);
        }
    }
    return items;
}

uint32_t Banker::depositedGold(Player& player) {
    const auto username = player.getName();
    if (!this->thePlayerHasAnAccount(username)) {
        this->createPlayerAccount(username);
        return 0;
    }
    return this->getGoldDepositedPlayer(username);
}
