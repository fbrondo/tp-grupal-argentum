#ifndef EQUIPMENT_H
#define EQUIPMENT_H

#include <vector>

#include "common/includes/protocol.h"
#include "server/includes/core/instances.h"

class Equipment {
private:
    std::vector<ItemInstance*> equipment_container;

    void equipHandItem(ItemInstance* item_inst);
    void equipItemDefensive(ItemInstance* item_inst);

public:
    Equipment(const Equipment&) = delete;             // No permitir copias
    Equipment& operator=(const Equipment&) = delete;  // No permitir igualaciones

    Equipment(Equipment&& other) noexcept = default;
    Equipment& operator=(Equipment&& other) noexcept = default;

    Equipment(/* args */);
    ~Equipment();

    void equipItem(ItemInstance* instance);

    /*Vendio, solto el item o lo quito del equipo*/
    void removeItem(size_t slot_index);

    /*Necesario para calcular los puntos de defensa*/
    std::vector<TypeItem> getEquipmentDefensive() const;
    std::vector<TypeItem> getEquipment() const;

    TypeItem getHandItem() const;
};


#endif
