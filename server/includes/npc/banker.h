#ifndef BANKER_H
#define BANKER_H

#include <cstdint>
#include <map>
#include <optional>
#include <string>

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
    //std::map<Id, Account> bank;
    Bank& bank;

    bool thePlayerHasAnAccount(const std::string& username);
    void createPlayerAccount(const std::string& player);
    std::optional<size_t> hasItemInAccountPlayer(const std::string& username, TypeItem type);

    void setItemSafeBox(const ShopItem *item);
    void incrementSlotSafeBox(TypeItem type_item);


public:
    Banker(TypeNPC type, const std::string& name, Bank& bank/*,const Pose& pos*/);

    void playerWithdrawItem(Player& player, TypeItem type);
    void playerDepositItem(Player& player, const ShopItem* item);
    void playerDepositGold(Player& player, uint32_t amount);
    void playerWithdrawGold(Player& player, uint32_t amount);
    ~Banker() override = default;
    InteractionResult interact() override;
};

#endif
