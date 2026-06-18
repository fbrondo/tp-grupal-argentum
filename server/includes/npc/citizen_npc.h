#pragma once

#include <map>
#include <memory>
#include <string>

#include "common/includes/core/snapshot.h"
#include "common/includes/types.h"
#include "server/includes/core/item.h"
#include "server/includes/core/map.h"
#include "server/includes/entity/entity.h"

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
    Pose pose;

public:
    CitizenNPC(TypeNPC type, const std::string& name, const Pose& pos);
    TypeNPC getTypeNPC() const;
    ~CitizenNPC() override = default;
    bool isAttackable() override { return false; }
    virtual CitizenNpcSnapshot getSnapshotCitizenNPC(const Id& npc_id);
};
