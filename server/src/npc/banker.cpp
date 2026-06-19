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

void Banker::setItemSafeBox(const std::string& username, const ShopItem* item) {
    std::vector<Slot>& slots = this->bank.accounts[username].safe_box;
    for (auto& slot: slots) {
        if (slot.isEmpty()) {
            slot.setItem(std::make_unique<ItemInstance>(item));
            slot.increase();
            break;
        }
    }
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
    const auto username = player.getUsername();
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
    ItemInstance item(slot.getItem());
    player.addItemToInventory(item);
    slot.decrease();
}

void Banker::playerDepositItem(Player& player, const ShopItem* item) {
    const auto username = player.getUsername();
    if (!this->thePlayerHasAnAccount(username)) {
        this->createPlayerAccount(username);
        return;
    }
    std::optional<size_t> index = this->hasItemInAccountPlayer(username, item->type);
    if (index.has_value()) {
        return; /*Excepcion de item no encontrado en la cuenta*/
    } else {
    }
}

// InteractionResult Banker::interact() {
//     InteractionResult result;
//     result.type = InteractionType::BANK_BOX;
//     result.open_bank = true;
//     return result;
// }
