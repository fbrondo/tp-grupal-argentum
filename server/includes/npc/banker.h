#ifndef BANKER_H
#define BANKER_H

#include <cstdint>
#include <map>
#include <optional>
#include <string>

#include "common/includes/types.h"
#include "server/includes/core/bank.h"
#include "server/includes/npc/citizen_npc.h"
#include "server/includes/player.h"


/*Banquero - Interaccion:
    - comprar
    - vender
    - listar
*/
class Banker: public CitizenNPC {
private:
    // std::map<Id, Account> bank;
    Bank& bank;

    bool thePlayerHasAnAccount(const std::string& username);
    void createPlayerAccount(const std::string& player);
    std::optional<size_t> hasItemInAccountPlayer(const std::string& username, TypeItem type);

    void setItemSafeBox(const std::string& username, const Item* item);
    void incrementSlotSafeBox(const std::string& username, TypeItem type_item);

    void increaseGoldAccount(const std::string& username, const uint32_t& amount);
    uint32_t decreaseGoldAccount(const std::string& username, const uint32_t& amount);

    uint32_t getGoldDepositedPlayer(const std::string& username) const;

public:
    explicit Banker(TypeNPC type, const std::string& name, Bank& bank, const Pose& pos);

    bool playerWithdrawItem(Player& player, TypeItem type);
    bool playerDepositItem(Player& player, TypeItem type);
    uint32_t playerDepositGold(Player& player, const uint32_t& amount);
    uint32_t playerWithdrawGold(Player& player, const uint32_t& amount);
    std::map<TypeItem, uint32_t> depositedItems(Player& player);
    uint32_t depositedGold(Player& player);

    virtual ~Banker() override{};
};

#endif
