#pragma once

#include <map>
#include <memory>
#include <string>

#include "../entity/entity.h"
#include "common/includes/types.h"
#include "server/includes/core/item.h"
#include "server/includes/core/map.h"
/*Representa un npc de ciudad/pueblo
    - Banquero
    - Sacerdote
    - Comerciante
*/
enum class InteractionType : uint8_t { TRADER_SHOP, BANK_BOX };

struct InteractionResult {
    InteractionType type;
    const std::map<TypeItem, std::unique_ptr<Item>>* trader_store = nullptr;
    bool open_bank = false;
};

class CitizenNPC: public Entity {
protected:
    TypeNPC type_npc;
    std::string name;

public:
    CitizenNPC(TypeNPC type, const std::string& name /*const Pose& pos*/);
    TypeNPC getTypeNPC();
    ~CitizenNPC() override = default;
    bool isAttackable() override { return false; }
};
