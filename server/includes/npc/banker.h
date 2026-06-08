#ifndef BANKER_H
#define BANKER_H

#include <cstdint>
#include <map>
// #include <memory>
#include <string>

#include "common/includes/types.h"
#include "server/includes//player.h"
#include "server/includes/npc/citizen_npc.h"


struct Account {
    uint32_t golden{0};
    std::map<Id, ItemInstance> safe_box;
    Account() = default;
};

/*Banquero - Interaccion:
    - comprar
    - vender
    - listar
*/
class Banker: public CitizenNPC {
private:
    std::map<Id, Account> bank;

public:
    Banker(TypeNPC type, const std::string& name /*,const Pose& pos*/);
    void createPlayerAccount(const Id& player_id);
    ~Banker() override = default;
    InteractionResult interact() override;
};

#endif
