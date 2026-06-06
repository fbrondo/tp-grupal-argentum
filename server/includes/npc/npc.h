#ifndef NPC_H
#define NPC_H

#include <map>
#include <memory>
#include <string>
#include "common/includes/types.h"
#include "server/includes/core/map.h"
#include "server/includes/entity.h"
#include "server/includes/core/item.h"

enum class InteractionType : uint8_t { TRADER_SHOP, BANK_BOX };

struct InteractionResult {
    InteractionType type;
    const std::map<TypeItem, std::unique_ptr<Item>>* trader_store = nullptr;
    bool open_bank = false;
};

class NPC: public Entity {
protected:
    TypeNPC type_npc;
    std::string name;

public:
    NPC(TypeNPC type, const std::string& name, Pose&& pos);
    TypeNPC getTypeNPC();
    virtual InteractionResult interact() = 0;  // Asi los npcs muestran su tienda/banco
    virtual ~NPC() = default;
};

#endif
