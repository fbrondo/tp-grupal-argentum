#ifndef BANKER_H
#define BANKER_H

#include <cstdint>
#include <map>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "common/includes/types.h"
#include "server/includes//player.h"
#include "server/includes/core/bank.h"
#include "server/includes/npc/citizen_npc.h"


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

    void setItemSafeBox(const std::string& username, const ShopItem* item);
    void incrementSlotSafeBox(const std::string& username, TypeItem type_item);


public:
    Banker(TypeNPC type, const std::string& name, Bank& bank, const Pose& pos);

    void playerWithdrawItem(Player& player, TypeItem type);
    void playerDepositItem(Player& player, const ShopItem* item);
    std::pair<std::vector<MsgItemInfo>, uint32_t> getBankContent(const std::string& username) const;
    ~Banker() override = default;
};

#endif
