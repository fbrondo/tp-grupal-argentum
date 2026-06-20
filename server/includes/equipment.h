#ifndef EQUIPMENT_H
#define EQUIPMENT_H

#include <memory>
#include <vector>

#include "common/includes/protocol.h"
#include "server/includes/core/instances.h"

class Equipment {
private:
    std::vector<std::unique_ptr<ItemInstance>> equipment_container;
    size_t getEquipmentIndex(const Item* item) const;

public:
    Equipment(const Equipment&) = delete;             // No permitir copias
    Equipment& operator=(const Equipment&) = delete;  // No permitir igualaciones

    Equipment(Equipment&& other) noexcept = default;
    Equipment& operator=(Equipment&& other) noexcept = default;

    Equipment(/* args */);
    ~Equipment();

    std::unique_ptr<ItemInstance> equipItem(std::unique_ptr<ItemInstance>&& instance);
    std::unique_ptr<ItemInstance> removeItem(size_t slot_index);

    /*Necesario para calcular los puntos de defensa*/
    std::vector<TypeItem> getEquipmentDefensive() const;
    std::vector<MsgSlot> getEquipmentSlots() const;

    TypeItem getHandItem() const;
    TypeItem getShieldItem() const;
    TypeItem getHelmetItem() const;
};


#endif
