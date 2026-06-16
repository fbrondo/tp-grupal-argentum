#include "server/includes/npc/banker.h"

Banker::Banker(TypeNPC type, const std::string& name,  Bank& bank_ /*, const Pose& pose_*/):
        CitizenNPC(type, name /*, pose_*/), bank(bank_) {}


void Banker::incrementSlotSafeBox(TypeItem type_item) {
    for (auto& slot : this->slots) {
        if (!slot.isEmpty() && slot.instance->item->type == type_item) {
            slot.increase();
            this->inv.size_current+=1;
            break;
        }
    }
}

void Banker::setItemSafeBox(const ShopItem *item) {
    for (auto& slot: this->slots) {
        if (slot.isEmpty()) {
            slot.instance = std::make_unique<ItemInstance>(item);
            slot.increase();
            break;
        }
    }
}

void Banker::createPlayerAccount(const std::string& username) {
    this->bank.accounts.emplace(username, Account());
}

std::optional<size_t>  Banker::hasItemInAccountPlayer(const std::string& username, TypeItem type) {
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

void Banker::playerWithdrawItem(Player &player, TypeItem type) {
    const auto username = player.getUsername();
    if (!this->thePlayerHasAnAccount(username)) {
        this->createPlayerAccount(username);
        return; /*Si se esta creando, obvio no lo va a tener*/
    }
    std::optional<size_t> index = this->hasItemInAccountPlayer(username, type);
    if (!index.has_value()) {
        return; /*Excepcion de item no encontrado en la cuenta*/
    }
    auto& slots = this->bank.accounts[username].safe_box;
    auto& slot = slots[index.value()];
    ItemInstance item(slot.getItem());
    player.addItemToInventory(item);
    slot.decrease();
}

void Banker::playerDepositItem(Player &player,  const ShopItem* item) {
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

InteractionResult Banker::interact() {
    InteractionResult result;
    result.type = InteractionType::BANK_BOX;
    result.open_bank = true;
    return result;
}
