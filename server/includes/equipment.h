#ifndef EQUIPMENT_H
#define EQUIPMENT_H

#include <tuple>
#include <vector>

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

    ItemInstance* equipItem(ItemInstance* item_inst);

    /*Vendio, solto el item o lo quito del equipo*/
    ItemInstance* removeItem(Id id_inst_item);

    /*Necesario para calcular los puntos de defensa*/
    std::vector<TypeItem> getEquipmentDefensive();

    /* Utilizo su arma para atacar o con un objeto magico lanzo algun hechizo, con el TypeItem el
     * server sabra que hacer */
    TypeItem getHandItem() const;
};


#endif
